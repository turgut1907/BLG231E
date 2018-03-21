// GIGI THE STRIKER
// Main reference: homework examples from ninova
// Image processing part(especially contour calculation part) is inspired from: http://harismoonamkunnu.blogspot.com.tr/2013/06/opencv-find-biggest-contour-using-c.html
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"
#include "opencv2/opencv.hpp"
#include "sensor_msgs/Image.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "nav_msgs/OccupancyGrid.h"
#include "gazebo_msgs/SetModelState.h"
#include "gazebo_msgs/GetModelState.h"
#include "gazebo_msgs/GetPhysicsProperties.h"
#include "gazebo_msgs/ModelState.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <vector>
using namespace std;
using namespace cv;
namespace enc = sensor_msgs::image_encodings;
#define PI 3.141
#define THRESH 0.4
#define SPINNUMBER 3

bool goflag = false;
bool goingcenter = false;
bool chatty_map=false;
int turncount = 0;
float robot_start_theta;
bool turnflag = false;
bool imageflag = false;
enum direction {LEFT,RIGHT};
direction turndirection;
bool turncorrection = false;
int targetcount = 0;
int imcount = 1;

ros::Publisher motor_command_publisher;
ros::Subscriber laser_subscriber;
ros::Subscriber map_subscriber;
ros::Subscriber image_subscriber;

sensor_msgs::LaserScan laser_msg;
sensor_msgs::Image image_msg;

nav_msgs::OccupancyGrid map_msg;
geometry_msgs::Twist motor_command;
ros::ServiceClient client;

class coord {
public:
    double x;
    double y;
};
vector <float> balls;
vector <coord> obs;
vector<coord> balls_c;
vector<coord> goal;
coord target;
int ball_center;
bool target_up = false;

float distance(coord &c1,coord &c2) { // distance between 2 points
  return sqrt((c1.x - c2.x) * (c1.x - c2.x) + (c1.y - c2.y) * (c1.y - c2.y));
}

long int map_index(int x,int y, int mapw) {
  return x + y*mapw;
}

float gettheta(string modelname) {
    gazebo_msgs::GetModelState m;
    m.request.model_name = modelname;
    client.call(m);
    double w = m.response.pose.orientation.w;
    double x = m.response.pose.orientation.x;
    double y = m.response.pose.orientation.y;
    double z = m.response.pose.orientation.z;
    float angle = atan2(2*x*y + 2*w*z, w*w + x*x - y*y - z*z);
    return angle;
}

coord getcoords(string modelname) {
    gazebo_msgs::GetModelState m;
    m.request.model_name = modelname;
    client.call(m);
    double x = m.response.pose.position.x;
    double y = m.response.pose.position.y;
    coord temp;
    temp.x = x;
    temp.y = y;
    return temp;
}

int find_ball(Mat pic) {
  Mat dst(pic.rows,pic.cols,CV_8UC1,Scalar::all(0));
	int largest_area=0;
	int largest_contour_index=0;
 	Rect bounding_rect;
	vector< vector<Point> > contours;
  vector<Vec4i> hierarchy;
  findContours(pic, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

  for( int i = 0; i< contours.size(); i++ )
      {
       double a=contourArea( contours[i],false);
       if(a>largest_area){
         largest_area=a;
         largest_contour_index=i;
         bounding_rect=boundingRect(contours[i]);
       }
      }
 	Scalar color( 255,255,255);
 	drawContours( dst, contours,largest_contour_index, color, CV_FILLED, 8, hierarchy );
 	cvtColor(dst, dst, CV_GRAY2RGB);
  vector<Moments> mu(contours.size());
  for( int i = 0; i < contours.size(); i++ ) mu[i] = moments(contours[i], false);

 	vector<Point2f> mc(contours.size());
  for( int i = 0; i < contours.size(); i++ ) {
      mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00);
      line(dst,Point(mc[i].x,0),Point(mc[i].x, dst.cols-1),Scalar(0,0,255),2,8);
      if (i == largest_contour_index) return mc[i].x;
  }
  return 320;
}
void image_callback(const sensor_msgs::ImageConstPtr& msg) {

    if (imageflag) {
        cv_bridge::CvImageConstPtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvShare(msg, enc::BGR8);
        }
        catch (cv_bridge::Exception& e) {
            ROS_ERROR("cv_bridge exception: %s", e.what());
            return;
        }
        Mat mask;
        Mat org = cv_ptr->image.clone();
        Scalar lowerb = cv::Scalar(16, 48, 180);
        Scalar upperb = cv::Scalar(32, 120, 255);
        inRange(cv_ptr->image, lowerb, upperb, mask);   // without light >> ball BGR(18,66,222)

        Scalar color = Scalar(255,0,0);
        float theta = gettheta("mobile_base");

        bool found = false;
        //ros::Duration(0.5).sleep();
        if (turncorrection) {
          int ball_cm =  find_ball(mask);
          if(ball_cm != 320) {
          line(mask,Point(ball_cm,0),Point(ball_cm, mask.cols-1),color,2,8);
          ball_center = ball_cm;
          if (ball_cm > mask.cols/2) turndirection = RIGHT;
          else turndirection = LEFT;
          if (abs(ball_center - 320) > 20) {
            cout << "Turn Correction | Ball Center: " << ball_center << endl;
            if (ball_center != 0) {
              motor_command.linear.x= 0;
              double turn;
              if (turndirection == LEFT) turn = 0.03;
              else turn = -0.03;
              if (turn < 0) cout << "Turning right..." << endl;
              else cout << "Turning left..." << endl;
              motor_command.angular.z=turn;
              motor_command_publisher.publish(motor_command);
              ros::Duration(0.1).sleep();
          }
        }
        else {
          target_up = false;
          imageflag = false;
          turncorrection = false;
          motor_command.linear.x= 0;
          motor_command.angular.z = 0;
          motor_command_publisher.publish(motor_command);
          ros::Duration(0.5).sleep();
          cout << "Lets shoot! Press a button!" << endl;
          getchar();
          motor_command.linear.x= 1;
          motor_command.angular.z = 0;
          motor_command_publisher.publish(motor_command);
          ros::Duration(1).sleep();
          cout << "Goal! I Hope." << endl;
          motor_command.linear.x= -1;
          motor_command.angular.z = 0;
          motor_command_publisher.publish(motor_command);
          ros::Duration(1).sleep();
          goingcenter = true;
        }
        }
        else {
          ros::Duration(0.5).sleep();
          double robot_theta =  gettheta("mobile_base");
          double angular =  atan2(goal[2].y-target.y,goal[2].x-target.x);
          if (abs(robot_theta-angular) > 0.2) {
            if (robot_theta > 3.14) robot_theta = robot_theta - 6.28;
            if (angular > 3.14) angular = angular - 6.28;
            double turn;
            if (angular - robot_theta > 3.14) turn = angular - robot_theta - 6.28;
            else if (angular - robot_theta < -3.14) turn = angular - robot_theta + 6.28;
            else turn = angular - robot_theta;
            motor_command.linear.x= 0;
            motor_command.angular.z= turn;
            motor_command_publisher.publish(motor_command);
            ros::Duration(0.1).sleep();
          }
          else {
          target_up = false;
          imageflag = false;
          turncorrection = false;
          motor_command.linear.x= 0;
          motor_command.angular.z = 0;
          motor_command_publisher.publish(motor_command);
          cout << "Lets shoot! Press a button!" << endl;
          getchar();
          motor_command.linear.x= 1;
          motor_command.angular.z = 0;
          motor_command_publisher.publish(motor_command);
          ros::Duration(1).sleep();
          cout << "Goal! I Hope." << endl;
          motor_command.linear.x= -1;
          motor_command.angular.z = 0;
          motor_command_publisher.publish(motor_command);
          ros::Duration(1).sleep();
          goingcenter = true;
          }
        }
      }
    }
  ros::Duration(0.1).sleep();
}

void laser_callback(const sensor_msgs::LaserScan::ConstPtr& msg) {
    coord t = getcoords("mobile_base");
    laser_msg=*msg;
    float turn = PI / 16;
    if (!turnflag) {
        motor_command.linear.x=0;
        motor_command.angular.z=turn;
        float current_theta = gettheta("mobile_base");
        motor_command_publisher.publish(motor_command);
        if (current_theta < robot_start_theta + 0.2 && current_theta > robot_start_theta -0.2) ros::Duration(1.5).sleep();
        else ros::Duration(0.1).sleep();
        //else ros::Duration(0.2).sleep();
        cout << "> Current Theta: " << current_theta << '\t' << "| Turn" << '\t' << turncount << "/" << SPINNUMBER << endl;
        if ( abs(current_theta - robot_start_theta) < turn/2 && turncount++ == SPINNUMBER) {
         turnflag = true;
         chatty_map = true;
         imageflag = false;
         ros::Duration(1).sleep();
     }
     else {
        motor_command.linear.x=0;
        motor_command.angular.z=0;
        motor_command_publisher.publish(motor_command);
     }

 }
 else if (goflag) {
    coord r = getcoords("mobile_base");
    if(target_up) {
  if ( distance(target,r) > 0.1) {
    double robot_theta = gettheta("mobile_base");
    double angular =  atan2(target.y-r.y,target.x-r.x); // we should go this way, but we are looking at robot_theta. so we should turn by difference
    if (robot_theta > 3.14) robot_theta = robot_theta - 6.28;
    if (angular > 3.14) angular = angular - 6.28;
    double turn;
    if (angular - robot_theta > 3.14) turn = angular - robot_theta - 6.28;
    else if (angular - robot_theta < -3.14) turn = angular - robot_theta + 6.28;
    else turn = angular - robot_theta;
    motor_command.linear.x= (distance(target,r) - (distance(target,r) * (abs(turn)/ 3.14))) * 0.6;
    motor_command.angular.z= turn;
    motor_command_publisher.publish(motor_command);
    ros::Duration(0.3).sleep();
  }
  else if(!imageflag){
    cout << "Turning to the goal..." << endl;
    double robot_theta = gettheta("mobile_base");
    double angular =  atan2(goal[2].y-target.y,goal[2].x-target.x);
    cout << "Angular: " << angular << " Current: " << robot_theta << endl;
    if (robot_theta > 3.14) robot_theta = robot_theta - 6.28;
    if (angular > 3.14) angular = angular - 6.28;
    double turn;
    if (angular - robot_theta > 3.14) turn = angular - robot_theta - 6.28;
    else if (angular - robot_theta < -3.14) turn = angular - robot_theta + 6.28;
    else turn = angular - robot_theta;
    motor_command.linear.x= 0;
    motor_command.angular.z= turn;
    motor_command_publisher.publish(motor_command);
    ros::Duration(0.5).sleep();
    imageflag = true;
    robot_theta = gettheta("mobile_base");
    cout << "current robot theta = " << robot_theta << endl;
    turncorrection = true;
  }
}
// go to the center
else if(goingcenter) {
  coord center;
  center.x = 2;
  center.y = target.y / 2;
  if ( distance(center,r) > 0.2) {
    double robot_theta = gettheta("mobile_base");
    double angular =  atan2(center.y-r.y,center.x-r.x);
    if (robot_theta > 3.14) robot_theta = robot_theta - 6.28;
    if (angular > 3.14) angular = angular - 6.28;
    double turn;
    if (angular - robot_theta > 3.14) turn = angular - robot_theta - 6.28;
    else if (angular - robot_theta < -3.14) turn = angular - robot_theta + 6.28;
    else turn = angular - robot_theta;
      motor_command.linear.x= (distance(center,r) - (distance(center,r) * (abs(turn)/ 3.14))) * 0.6;
      motor_command.angular.z= turn;
      motor_command_publisher.publish(motor_command);
      ros::Duration(0.3).sleep();
      }
      else {
          cout << "Lets look for another ball!" << endl;
          goingcenter = false;
      }
    }
 }
}
void map_callback(const nav_msgs::OccupancyGrid::ConstPtr& msg) {
    map_msg=*msg;
    double map_width=map_msg.info.width;
    double map_height=map_msg.info.height;

    double map_origin_x = map_msg.info.origin.position.x;
    double map_origin_y = map_msg.info.origin.position.y;
    double map_res = map_msg.info.resolution;
    double map_orientation = acos(map_msg.info.origin.orientation.z);

    std::vector<signed char> map = map_msg.data;

    if(chatty_map) {
        std::cout<<"Obstacle coordinates are saving..."<<std::endl;
        ofstream map_text("map.txt");
        ofstream map_ind("map_index.txt");
        vector <int> rows,cols;
        int up_row,down_row,left_col,right_col;
        for(unsigned int x=0; x<map_width; x+=1) {
            for(unsigned int y=0; y<map_height; y+=1) {
                unsigned int index = map_index(x,y,map_width);
                bool foundrow = true,foundcol = true;
                if (x < map_width-15 && y < map_height-15) {
                  for (int i=0;i < 10;i++) {
                    if (!(map[map_index(x,y+i,map_width)] > 0)) foundrow = false;
                    if (!(map[map_index(x+i,y,map_width)] > 0)) foundcol= false;
                  }
                  if (foundrow && find (rows.begin(), rows.end(), x) == rows.end()) rows.push_back(x);
                  if (foundcol && find (cols.begin(), cols.end(), y) == cols.end()) cols.push_back(y);
              }
              sort(rows.begin(),rows.end());
              sort(cols.begin(),cols.end());

              for(int i = 0; i < cols.size(); i ++) {
                if (cols[i+1] - cols[i] > 1) {
                    left_col = cols[i];
                    right_col = cols[i+1];
                    break;
                }
              }
              bool pass = false;
              for(int i = 0; i < rows.size(); i ++) {
                if (!pass && rows[i+1] - rows[i] > 1) {
                  pass = true;
                  continue;
                 }
                if (pass && rows[i+1] - rows[i] > 1) {
                    up_row = rows[i];
                    down_row = rows[i+1];
                    break;
                }
              }
                map_ind << map[index] << " ";
                if (map[index] > 0) {
                    map_text << "*";
                    double ox = (x * map_res) + map_origin_x;
                    double oy = (y * map_res) + map_origin_y;
                    coord temp;
                    temp.x = ox;
                    temp.y = oy;
                    obs.push_back(temp);
                }
                else map_text << " ";
            }
            map_text << endl;
            map_ind << endl;
        }
    map_text.close();
    map_ind.close();
    chatty_map = false;
    cout << "Obstacle saving is done!" << endl;
    ofstream text("obstacles.txt");
    cout << "Vector is writing to the text file..." << endl;
    double goal_x=-55;
    double goal_y1 = -55;
    double goal_y2 = 55;
    for(int i=0; i < obs.size()-1; i++) {
        text << obs[i].x << "," << obs[i].y << endl;
        if (abs(obs[i].x - obs[i+1].x) < THRESH && abs(obs[i].y - obs[i+1].y) > 2 * THRESH && abs(obs[i].y - obs[i+1].y) < 5 * THRESH ) {
            cout << obs[i].x << "," << obs[i].y << " <--> " << obs[i+1].x << "," << obs[i+1].y << endl;
            if (obs[i].x > -3) {}//
            else {
                if (goal_x < obs[i].x) goal_x = obs[i].x;
                if (goal_y1 < obs[i].y) goal_y1 = obs[i].y;
                if (goal_y2 > obs[i+1].y) goal_y2 = obs[i+1].y;
            }
          }
        }

    text.close();
    coord t1,t2,t3;
    t1.x = goal_x;
    t2.x = goal_x;
    t3.x = goal_x;
    t1.y = goal_y1;
    t2.y = goal_y2;
    t3.y = (goal_y1 + goal_y2) / 2;
    cout << "Goal: " << t3.x << "," << t3.y << endl;
    goal.push_back(t1);
    goal.push_back(t2);
    goal.push_back(t3);
    for(unsigned int x=up_row+2; x<down_row-2; x+=1) {
        for(unsigned int y=left_col+2; y<right_col-2; y+=1) {
            unsigned int index = map_index(x,y,map_width);
            if (map[index] > 0) {
              int count = 0;
               if (map[map_index(x+1,y,map_width)] > 0) count++;
               if (map[map_index(x-1,y,map_width)] > 0) count++;
               if (map[map_index(x,y-1,map_width)] > 0) count++;
               if (map[map_index(x,y+1,map_width)] > 0) count++;

               double ox = (x * map_res) + map_origin_x;
               double oy = (y * map_res) + map_origin_y;
               coord t;
               t.x = ox;
               t.y = oy;
               if (count > 1 && ( balls_c.size() == 0 || (balls_c.size() != 0 && distance(balls_c[balls_c.size()-1],t) > 0.3))) {
                 double ox = (x * map_res) + map_origin_x;
                 double oy = (y * map_res) + map_origin_y;
                 coord temp;
                 temp.x = ox;
                 temp.y = oy;
                 balls_c.push_back(temp);
               }
            }
          }
        }
    cout << "Balls: " << endl;
    for(int i=0; i < balls_c.size(); i++) {
      cout << balls_c[i].x << " - " << balls_c[i].y << endl;
    }
    goflag = true;
    //ros::Duration(1.5).sleep();
    }
    if (goflag && !target_up && !goingcenter) {
      cout << "Target Count: " << targetcount << " Ball Count: " << balls_c.size() << endl;
      if (targetcount >= balls_c.size()) {
        cout << "No Balls Left :( Gigi is hungry now." << endl;
         ros::shutdown();
         return;
      }
      double x1 = goal[2].x;
      double y1 = goal[2].y;
      double x2 = balls_c[targetcount].x;
      double y2 = balls_c[targetcount].y;
      double x3,y3;
      float d = distance(balls_c[targetcount++],goal[2]);
      y3 = y2 + (((y2 - y1) / d) * 0.5);
      x3 = x2 + (((x2 - x1) / d) * 0.5);
      target.x = x3;
      target.y = y3;
      cout << "Target: " << x3 << "," << y3 << endl;
      target_up = true;
      cout << "Press a button..." << endl;
      getchar();
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "footballer");
    ros::NodeHandle n;
    ros::NodeHandle m;
    client = m.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
    gazebo_msgs::GetModelState getmodelstate;
    getmodelstate.request.model_name = "mobile_base";

    robot_start_theta = gettheta("mobile_base");
    cout << "Robot Start Theta: " << robot_start_theta << endl;
    motor_command_publisher = n.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/navi", 100);

    laser_subscriber = n.subscribe("/scan", 1000,laser_callback);
    image_subscriber = n.subscribe("/camera/rgb/image_raw",1000,image_callback);
    map_subscriber = n.subscribe("/map", 1000,map_callback);

    ros::Duration time_between_ros_wakeups(0.1);
    while(ros::ok()) {
        ros::spinOnce();
        time_between_ros_wakeups.sleep();
    }
    return 0;
}
