#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/cred.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/file.h>
#include <asm/switch_to.h>		/* cli(), *_flags */
#include <asm/uaccess.h>	/* copy_*_user */
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include "messagebox_ioctl.h"

int boxlimit = DEF_BOXLIMIT; //unreaded message limit
int readflag = 0; // for read buffer clean

int messagebox_major = MESSAGEBOX_MAJOR;
int messagebox_minor = 0;

module_param(boxlimit,int, S_IRUGO);
module_param(messagebox_major, int, S_IRUGO);
module_param(messagebox_minor, int, S_IRUGO);

MODULE_AUTHOR("Turgut Can Aydinalev,Beste Burcu Bayhan");
MODULE_LICENSE("Dual BSD/GPL");

struct msg { //includes one message
    char text[MES_SIZE];
    int read;
    struct msg *next;
};

struct inbox { //includes the inbox of one user
    int uid;
    char username[USERNAME_SIZE];
    struct msg *head;
    int mescount;
    int unreadcount;
};

struct messagebox_dev { //includes general structure of all users
    struct inbox users[USERLIMIT];
    int usercount;
    struct semaphore sem;
    struct cdev cdev;
    int r_mode;
};

struct messagebox_dev mbox;

int messagebox_trim(void) { //free the memory
    int i;
    for(i=0; i <mbox.usercount;i++) {
        if (mbox.users[i].head != NULL) {
            while(mbox.users[i].head != NULL) {
                struct msg *temp = mbox.users[i].head;
                mbox.users[i].head = mbox.users[i].head->next;
                kfree(temp);
            }
        }
    }
    return 0;
}


int messagebox_open(struct inode *inode, struct file *filp)
{
  
    printk(KERN_INFO"OPENING\n");
    filp->private_data = &mbox;
    return 0;
}


int messagebox_release(struct inode *inode, struct file *filp)
{
    return 0;
}

int check_create_user(int current_uid, char username[USERNAME_SIZE]) {

    int i;
    int flag = 0;
    for(i=0; i <= mbox.usercount;i++) { //searches the users if the current user exists or not
       
        if (mbox.users[i].uid == current_uid) {
            flag = 1;
            break;
        }
        else if(strcmp(mbox.users[i].username,username) == 0) {
            flag = 1;
            break;
        }
    }
    if (flag == 1) { //the current user has already created, returns the index
        return i; 
    }
    else { // user box not here. we should create it
        mbox.users[mbox.usercount].head = NULL;
        mbox.users[mbox.usercount].mescount = 0;
        mbox.users[mbox.usercount].unreadcount = 0;
        mm_segment_t ofs = get_fs();
        set_fs(get_ds()); //switch to kernel space
        struct file * filp = filp_open("/etc/passwd",O_RDONLY,0);
        if (IS_ERR(filp)) {
            printk(KERN_ALERT"/ETC/PASSWD ERROR\n");
            return;
        }
        filp->f_pos=0;       

        if (current_uid != -1) { //if we know the user id. we will proceed with it. if not. we will use username
            mbox.users[mbox.usercount].uid = current_uid;
            char temp[10];
            int charcount = 0;
            int tempuid = current_uid;
            while (tempuid > 9) { //convert uid to char
                temp[charcount++] = (tempuid % 10) + '0';
                tempuid = tempuid / 10;
            }
            temp[charcount++] = tempuid + '0';
            temp[charcount] = '\0';
            int i;
            int j;
            char buff[1];
            buff[0] = 10;
            char charuid[6];
            for (j=0,i=charcount-1; i > -1; j++,i--) charuid[i] = temp[j];
            charuid[charcount] = '\0';
            char tempuname[USERNAME_SIZE];
            i=0;
            while (i < 50) { //looks for 50 lines in the passwd file
                if (buff[0] == 10) { //next char is the first char of the line
                    i++;
                    int a=0;
                    for (a=0;a <USERNAME_SIZE-1; a++) tempuname[a] = 'a'; //dummy
                    tempuname[USERNAME_SIZE-1] = '\0';
                    a=0;

                    vfs_read(filp,buff,1,&filp->f_pos); //reads one character

                    while(buff[0] != ':') { //parse the username
                        tempuname[a++] = buff[0];
                        vfs_read(filp,buff,1,&filp->f_pos);
                    }
                    tempuname[a] = '\0';
                    vfs_read(filp,buff,1,&filp->f_pos);
                    vfs_read(filp,buff,1,&filp->f_pos);
                    vfs_read(filp,buff,1,&filp->f_pos); //now head of uid
                    int flag = 0;
                    int count=0;
                    while (charuid[count] != '\0') { //parse the uid
                        if(charuid[count++] != buff[0]) { //if it does not match it waits for the next line
                            flag = 1;
                            break;
                        }
                        vfs_read(filp,buff,1,&filp->f_pos);
                    }

                    if (flag == 0) { //found in passwd with uid
                        int count=0;
                        for(count=0;tempuname[count] != '\0'; count++) {
                            mbox.users[mbox.usercount].username[count] = tempuname[count];
                        }
                        mbox.users[mbox.usercount].username[count] = '\0';
                        mbox.usercount++;
                        set_fs(ofs);
                        return mbox.usercount-1;
                    }
                }
                
                vfs_read(filp,buff,1,&filp->f_pos);
            }
             printk(KERN_WARNING"I COULDN'T FIND THE USER. SORRY. \n");
             return -1;
        }
        else { //if uid is -1, the function will search with username
            strcpy(mbox.users[mbox.usercount].username, username);
            char buff[1];
            buff[0] = 10;
            int i=0;
            
            while (i < 50) { //looks for 50 lines in the passwd file
                int flag= 0;
                if (buff[0] == 10) { 
                    i++;
                    vfs_read(filp,buff,1,&filp->f_pos);

                    int count=0;
                    while(buff[0] != ':') { //parse the username
                        if (username[count++] != buff[0]) { //if it does not match it waits for the next line
                            flag = 1;
                            break;
                        }
                        vfs_read(filp,buff,1,&filp->f_pos);
                    }

                    if (flag == 0) {
                        vfs_read(filp,buff,1,&filp->f_pos);
                        vfs_read(filp,buff,1,&filp->f_pos);
                        vfs_read(filp,buff,1,&filp->f_pos); //now head of uid
                        char tempuid[10];
                        int count = 0;
                        while(buff[0] != ':') {
                            tempuid[count++] = buff[0];
                            vfs_read(filp,buff,1,&filp->f_pos);
                        }
                        tempuid[count] = '\0';
                        int temp[1];
                        kstrtoint(&tempuid,10,&temp[0]); //convert uid to int
                        mbox.users[mbox.usercount].uid = temp[0]; //initialize the uid to the user
                        mbox.usercount++;
                        set_fs(ofs); //switch back to old space
                        return mbox.usercount-1;
                    }
                }
                vfs_read(filp,buff,1,&filp->f_pos);
            } // while end
            printk(KERN_WARNING"I COULDN'T FIND THE USER. SORRY. \n");
            return -1;
        } // else end
    }
};


ssize_t messagebox_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
{
    if (readflag == 1) { //to prevent infinite writing to the user screen
        readflag = 0;
        return 0;
    }

    ssize_t retval = 0;
    struct messagebox_dev *dev = filp->private_data;
    printk(KERN_INFO"READING\n");
    printk(KERN_INFO"BOX LIMIT: %d\n", boxlimit);
    if (down_interruptible(&dev->sem)) return EBUSY;
    int uid = get_current_user()->uid.val;
    int i = check_create_user(uid,"aa");
    char *t;
    t = kmalloc(1000 * sizeof(char), GFP_KERNEL);
    strcpy(t,"Welcome, ");
    strcat(t, mbox.users[i].username);
    strcat(t,"!\n");

    if (mbox.r_mode == EXCLUDE_READ) { //only shows the unreaded messages
        if (mbox.users[i].unreadcount == 0) {
            strcat(t,"No new message...\n");
        }
        else {
            strcat(t,"Unread Messages:\n");
            struct msg *temp = mbox.users[i].head;
            while(temp != NULL) {
                if (temp->read == 1) {
                    strcat(t,temp->text);
                    temp->read = 0;
                }
                temp = temp->next;
            }
            mbox.users[i].unreadcount = 0;
        }
    }
    else { //shows all messages of the user
        if (mbox.users[i].mescount == 0) {
            strcat(t,"No messages at all...\n");
        }
        else {
            strcat(t,"All Messages:\n");
            struct msg *temp = mbox.users[i].head;
            while(temp != NULL) {
                strcat(t,temp->text);
                temp->read = 0;
                temp = temp->next;
            }
            mbox.users[i].unreadcount = 0;
        }
    }
    
    copy_to_user(buf,t,strlen(t)); //shows the messages to the user
    retval = strlen(t);
    kfree(t); 
     up(&dev->sem);
     readflag = 1;
     return retval;


}


int add_message(int to_index,int from_index,char *buff) {
    
    if (mbox.users[to_index].unreadcount >= boxlimit) { //checks the unreaded message limit
        printk(KERN_WARNING"Inbox of the receiver is full!\n");
        return -ENOMEM;
    }
    char *temp = kmalloc(MES_SIZE * sizeof(char), GFP_KERNEL);
    strcpy(temp,mbox.users[from_index].username);
    strcat(temp,": ");
    strcat(temp,buff); //takes the message
    strcat(temp,"\n");

    struct msg *newmsg = kmalloc(sizeof(struct msg),GFP_KERNEL); //creates a new message
    newmsg->read = 1; 
    strcpy(newmsg->text,temp);
    newmsg->next = NULL;

    if (mbox.users[to_index].head == NULL) mbox.users[to_index].head = newmsg; //adding the new message to the user's inbox
    else {
        struct msg *ptr;
        ptr = mbox.users[to_index].head;
        while (ptr->next != NULL) ptr = ptr->next;
        ptr->next = newmsg;
    }
    //increase the counters
    mbox.users[to_index].unreadcount ++; 
    mbox.users[to_index].mescount ++;
    kfree(temp);
    return 0;
}

ssize_t messagebox_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)
{
    printk(KERN_INFO"WRITING\n");
    int uid = get_current_user()->uid.val;
    struct messagebox_dev *dev = filp->private_data;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    int fromindex = check_create_user(uid,"dummy text"); // returns the index of the current user in the messagebox structure

    char *touname = kmalloc(USERNAME_SIZE * sizeof(char), GFP_KERNEL);
    char *t = kmalloc(1000 * sizeof(char), GFP_KERNEL);

    copy_from_user(t,buf,count); //takes the written message
    int i=1; // first char is @
    while (t[i] != ' ') { //parse the receiver username
        touname[i-1] = t[i];
        i++;
    }
    touname[i-1] = '\0';
    t = &t[i+1];
    i=0;
    while (t[i] != 10) i++; //parsed message
    t[i] = '\0';

    int toindex = check_create_user(-1,touname); //returns the index of the receiver user
    if (toindex< 0) return EINVAL;

    int err = add_message(toindex,fromindex,t); //add message to the user inbox
    kfree(t);
    kfree(touname);
    up(&dev->sem);
    if (err < 0) return err; // user limit exceed
    return count;

}

long messagebox_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{ 
    int uid = get_current_user()->uid.val;
    char *temp;
    int i,flag;
    if (uid != 0) return EACCES; //if the current user is not superuser , it returns permission denied error
    switch(cmd) { //select the function type
        case SET_READ: //read mode edit
            if (arg != EXCLUDE_READ && arg != INCLUDE_READ) return EINVAL;
            mbox.r_mode = arg;
            return 0;
        case SET_LIMIT: //edit the message box limit
            if(arg < 1) return EINVAL; //limit cannot be smaller than 1 
            boxlimit = arg;
            return 0;
        case DELETE_MSG: //delete all messages of the given user
            temp = (char *)arg;
            i=0;
            flag =0;
            for(i=0; i <= mbox.usercount; i++) {
                if (strcmp(mbox.users[i].username,temp) == 0) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1) { //user found 
                struct msg *tmp;
                while (mbox.users[i].head) {
                    tmp = mbox.users[i].head;
                    mbox.users[i].head = mbox.users[i].head->next;
                    kfree(tmp);
                }
                mbox.users[i].head = NULL;
                mbox.users[i].mescount = 0;
                mbox.users[i].unreadcount = 0;
                return 0;
            }

            else return EINVAL; //user not found
        default:
            return EINVAL; //wrong function type
    }

}


loff_t messagebox_llseek(struct file *filp, loff_t off, int whence)
{
    struct messagebox_dev *dev = filp->private_data;
    loff_t newpos;

    filp->f_pos = newpos;

    return newpos;
}


struct file_operations messagebox_fops = {
    .owner =    THIS_MODULE,
    .llseek =   messagebox_llseek,
    .read =     messagebox_read,
    .write =    messagebox_write,
    .unlocked_ioctl =  messagebox_ioctl,
    .open =     messagebox_open,
    .release =  messagebox_release,
};


void messagebox_cleanup_module(void)
{
    printk(KERN_INFO"CLEANING\n");
    dev_t devno = MKDEV(messagebox_major, messagebox_minor);
    messagebox_trim();
    cdev_del(&mbox.cdev);
    unregister_chrdev_region(devno, 1);
}


int messagebox_init_module(void)
{
    printk(KERN_INFO"INIT MODULE\n");
    int result, i;
    int err;
    dev_t devno = 0;
    struct messagebox_dev *dev;

    if (messagebox_major) {
        devno = MKDEV(messagebox_major, messagebox_minor);
        result = register_chrdev_region(devno, 1, "messagebox");
    } 
    else {
        result = alloc_chrdev_region(&devno, messagebox_minor, 1,"messagebox");
        messagebox_major = MAJOR(devno);
    }
    if (result < 0) {
        printk(KERN_WARNING "messagebox: can't get major %d\n", messagebox_major);
        return result;
    }
    /* Initialize each device. */
        mbox.usercount = 0;
        mbox.r_mode = EXCLUDE_READ;
        sema_init(&mbox.sem,1);
        cdev_init(&mbox.cdev, &messagebox_fops);
        mbox.cdev.owner = THIS_MODULE;
        mbox.cdev.ops = &messagebox_fops;
        err = cdev_add(&mbox.cdev, devno, 1);
        if (err)
            printk(KERN_NOTICE "Error %d adding messagebox%d", err, i);

    return 0; /* succeed */


}

module_init(messagebox_init_module);
module_exit(messagebox_cleanup_module);
