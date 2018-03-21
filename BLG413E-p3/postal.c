// SYSTEM PROGRAMMING PROJECT 3 - TURGUT CAN AYDINALEV & BESTE BURCU BAYHAN
#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <time.h>

static char csvloc[200];

time_t mtime;

struct node {
	char code[10];
	char n[35];
	char city[20];
	char dist[30];
	float lat;
	float lon;
};

unsigned long int ncount = 0;
struct node arr[37000];

void add_node(char *s1,char *s2,char *s3,char *s4,float f1,float f2) {
	strcpy(arr[ncount].code,s1);
	strcpy(arr[ncount].n,s2);
	strcpy(arr[ncount].city,s3);
	strcpy(arr[ncount].dist,s4);
	arr[ncount].lat = f1;
	arr[ncount].lon = f2;
	ncount++;
	return;
}

void print_node(unsigned long int k) {
	if (ncount < k) return;
	else {
		printf("%s %s %s %s %f %f\n",arr[k].code,arr[k].n,arr[k].city,arr[k].dist,arr[k].lat,arr[k].lon);
		return;
	}
}

int writefile(void) {
	FILE *dosya = fopen(csvloc, "w+");
	long int i=0;
	if (dosya == NULL) return -1;
	for(i=0;i <ncount; i++) {
		fprintf(dosya,"%s\t%s\t%s\t%s\t%.4f\t%.4f\n",arr[i].code,arr[i].n,arr[i].city,arr[i].dist,arr[i].lat,arr[i].lon);
	}
}


int readfile(void) {
	FILE *dosya = fopen(csvloc, "r");
	int i=0;
	ncount = 0;
	if (dosya == NULL) {
		printf("File is not found!\n");
		return -1;
	}
	else {
		while(!feof(dosya)) {
			char str1[20];
			char str2[20];
			char str3[20];
			char str4[20];
			char str5[20];
			char str6[20];
			fscanf(dosya,"%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\n]\n",str1,str2,str3,str4,str5,str6);
			add_node(str1,str2,str3,str4,atof(str5),atof(str6));
			if (feof(dosya)) break;
		}
		fclose(dosya);
		return 0;
	}
}

char * getcityname(char *path) { // parse city name from path
	char *temp = malloc(50 * sizeof(char));
	int tcount = 0;
	char *ptr;
	ptr = path;
	ptr++; // first one is '/'
	while (*ptr !='/') ptr++;
	ptr++;
	int i=0;
	for(i=0;i <strlen(ptr);i++) {
		if (ptr[i] != '\0' && ptr[i] != '/') temp[tcount++] = ptr[i];
		else break;
	}
	temp[tcount] = '\0';
	return temp;
}

char * getpostcode(char *path) { // parse post code name from path
	char *temp = malloc(50 * sizeof(char));
	int tcount = 0;
	char *ptr;
	ptr = path;
	ptr++; // first one is '/'
	while (*ptr !='/') ptr++; // city code
	ptr++;
	while (*ptr !='/') ptr++;
	ptr++;
	int i=0;
	for(i=0;i <strlen(ptr);i++) {
		if (ptr[i] != '\0' && ptr[i] != '/' && ptr[i] != '.') temp[tcount++] = ptr[i];
		else break;
	}
	temp[tcount] = '\0';
	return temp;
}

char * getnname(char *path) { // parse neighborhood name from path
	char *temp = malloc(50 * sizeof(char));
	int tcount = 0;
	char *ptr;
	ptr = path;
	ptr++; // first one is '/'
	while (*ptr !='/') ptr++; // city
	ptr++;
	while (*ptr !='/') ptr++; // dist
	ptr++;
	while (*ptr !='/') ptr++;
	ptr++;
	int i=0;
	for(i=0;i <strlen(ptr);i++) {
		if (ptr[i] != '\0' && ptr[i] != '/' && ptr[i] != '.') temp[tcount++] = ptr[i];
		else break;
	}
	temp[tcount] = '\0';
	return temp;
}


char * getdistname(char *path) { // parse district name from path
	char *temp = malloc(50 * sizeof(char));
	int tcount = 0;
	char *ptr;
	ptr = path;
	ptr++; // first one is '/'
	while (*ptr !='/') ptr++; // city
	ptr++;
	while (*ptr !='/') ptr++; // dist
	ptr++;
	int i=0;
	for(i=0;i <strlen(ptr);i++) {
		if (ptr[i] != '\0' && ptr[i] != '/') temp[tcount++] = ptr[i];
		else break;
	}
	temp[tcount] = '\0';
	return temp;
}

long int validate_path(char *path) { // check path is true or not for some conditions
	if (strncmp(path,"/NAMES",6) == 0) {
		char *c;
		char *d;
		char *n;
		d = getdistname(path);
		c = getcityname(path);
		n = getnname(path);
		unsigned int i;
		for(i=0; i < ncount; i++) {
			if (strcmp(arr[i].city,c) == 0 && strcmp(arr[i].n,n) == 0 && strcmp(arr[i].dist,d) == 0) {
				free(d);
				free(c);
				free(n);
				return i; // found in index i
			}
		}
		free(d);
		free(c);
		free(n);
		return -1; // not found
	}
	else if (strncmp(path,"/CODES",6) == 0) {
		char *p;
		p = getpostcode(path);
		unsigned int i;
		for(i=0; i < ncount; i++) {
			if (strcmp(arr[i].code,p) == 0) {
				free(p);
				return i;
			}
		}
		free(p);
		return -1;
	}
	else return -1;
}

static int postal_getattr(const char *path, struct stat *stbuf)
{
	struct stat ptr;
	lstat(csvloc,&ptr);
	if (mtime != ptr.st_mtime) { // check modification time
		mtime = ptr.st_mtime; // update it
		readfile(); // read file again
	}
    int res = 0;
    printf("postal_getattr | %s \n",path);
    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755; // folder
        stbuf->st_nlink = 2;
    }
    else if (strncmp(path, "/NAMES",6) == 0) { // start with /names
    	if (strcmp(path, "/NAMES") == 0) { // equal to /names
        stbuf->st_mode = S_IFDIR | 0755; // folder
        stbuf->st_nlink = 2;
    	}
    	else {
    		char *ptr = path + 6;
    		int flag = 0;
    		unsigned long int i = 0;
    		for(i=0; i < ncount; i++) {
    			char temp[50];
    			strcpy(temp,"/");
    			strcat(temp,arr[i].city);
    			if (strncmp(ptr,temp,strlen(arr[i].city) + 1) == 0) { // starts with a city name
		    		if (strcmp(ptr,temp) == 0) { // nothing more
		    			stbuf->st_mode = S_IFDIR | 0755; // folder
		        		stbuf->st_nlink = 2;
	    			} // if
	    			else { // there is more than city name
	    				strcat(temp,"/");
	    				strcat(temp,arr[i].dist);
	    				if (strncmp(ptr,temp, strlen(temp)) == 0) { // disc name
	    					if (strcmp(ptr,temp) == 0) {
	    					stbuf->st_mode = S_IFDIR | 0755; // folder
		        			stbuf->st_nlink = 2;
		        			}
		        			else { // if it is longer than disc, it should be file
		        				long int j = validate_path(path);
		        				if ( j == -1) res = -ENOENT;
		        				else {
		        					stbuf->st_mode = S_IFREG | 0444; // file
									stbuf->st_nlink = 1;
									stbuf->st_size = strlen(arr[j].code) + strlen(arr[j].city) + strlen(arr[j].dist) + strlen(arr[j].n) + 77; // 77 is constant character count in the file
								}
		        			}
		        		}
	    			}
    			} // sehir adi ifi
    		} // for

   		} //else
   	}
   	else if (strncmp(path, "/CODES",6) == 0) { // start with /codes
   		if  (strcmp(path, "/CODES") == 0) { // equal to /codes
   			stbuf->st_mode = S_IFDIR | 0755; // folder
        	stbuf->st_nlink = 2;
   		}
   		else { // there is more than /codes
   			char *ptr = path + 7;
   			unsigned long int i = 0;
    		for(i=0; i < ncount; i++) {
   				if (strncmp(ptr,arr[i].code,2) == 0) {
   					char temp[5];
   					strncpy(temp,arr[i].code,2);
   					temp[2] = '\0';

   					if (strcmp(ptr,temp) == 0) {
   						stbuf->st_mode = S_IFDIR | 0755; // folder
		        		stbuf->st_nlink = 2;
		        	}
		        	else {
		        		long int j = validate_path(path);
		        		if ( j == -1) res = -ENOENT;
		        		
		        		else {
		        			stbuf->st_mode = S_IFREG | 0444; // file
							stbuf->st_nlink = 1;
							stbuf->st_size = strlen(arr[j].code) + strlen(arr[j].city) + strlen(arr[j].dist) + strlen(arr[j].n) + 77;
		        		}
		        	}
   				}
   			}
   		}
   }
    return res;
}


static int postal_readdir(const char *path, void *buf, fuse_fill_dir_t filler,off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;
    printf("postal_readdir | %s\n",path);
    if (strcmp(path, "/") == 0) {
     	filler(buf, ".", NULL, 0);
    	filler(buf, "..", NULL, 0);
    	filler(buf, "NAMES", NULL, 0);
    	filler(buf, "CODES", NULL, 0);
    	return 0;
    }

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    unsigned long int i;

    for(i=0; i < ncount-1; i++) {
    	if (strncmp(path,"/NAMES",6) == 0) {
	    	if (strcmp(path,"/NAMES") == 0 ) {
	    		if (strcmp(arr[i].city,arr[i+1].city) != 0) filler(buf,arr[i].city, NULL, 0);
	    	}
    		else {
    			char *ptr = path + 6;
    			char temp[50];
    			strcpy(temp,"/");
    			char *t;
	    		t = getcityname(path);
    			strcat(temp,t);
    			if (strncmp(ptr,temp, strlen(temp)) == 0) { // path fits or not
    				if(strcmp(t,arr[i].city) == 0 && strcmp(temp,ptr) == 0 && strcmp(arr[i].dist,arr[i+1].dist) != 0) {
    					long int a = i;
    					int flag = 0;
    					while (strcmp(arr[i].dist,arr[a].dist) == 0) a--; // check the district folder exists before or not

    					while ( a > -1 && strcmp(arr[i].city,arr[a].city) == 0 ) {
    						if (strcmp(arr[i].dist,arr[a].dist) == 0) flag = 1;
    						a--;
    					}
    					if (flag == 0) filler(buf,arr[i].dist, NULL, 0); // if not, create it
    				} 
    				else {
    					 char *d;
	    				 d = getdistname(path);
	    				if (strcmp(arr[i].city,t) == 0 && strcmp(arr[i].dist,d) == 0) {
	    				 	char r[60];
	    				 	strcpy(r,arr[i].n);
	    				 	strcat(r,".txt"); // file names
	    				 	filler(buf,r, NULL, 0);
	    				}
	    				free(d);
    				}
    			}
    			free(t);
    		}
    }
    	else if (strncmp(path,"/CODES",6) == 0) {
    		if(strcmp(path,"/CODES") == 0) {
    			if (strcmp(arr[i].city,arr[i+1].city) != 0) {
    				char temp[5];
    				strncpy(temp,arr[i].code,2);
    				temp[2] = '\0';
    				filler(buf,temp, NULL, 0);
    			}
    		}
    		else {
    			char *ptr = path + 7;
    			if (strncmp(ptr,arr[i].code,2) == 0 && strcmp(arr[i].code,arr[i+1].code) != 0) {
    				char temp[15];
    				strcpy(temp,arr[i].code);
    				strcat(temp,".txt");
    				filler(buf,temp, NULL, 0);
    			}
    		}
    	}
	}
    return 0;
}

static int postal_open(const char *path, struct fuse_file_info *fi)
{
    if (validate_path(path) == -1) // if path is wrong, you cannot open the file
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int postal_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    long int i = validate_path(path);
    printf("postal_read | path: %s\n",path);
    if (i == -1)
        return -ENOENT;

    char temp[250];
    strcpy(temp,"code: ");
    strcat(temp,arr[i].code);
    strcat(temp,"\nneighborhood: ");
    strcat(temp,arr[i].n);
    strcat(temp,"\ncity: ");
    strcat(temp,arr[i].city);
    strcat(temp,"\ndistrict: ");
    strcat(temp,arr[i].dist);
    strcat(temp,"\nlatitude: ");
    char t[10];
    sprintf(t,"%.4f",arr[i].lat);
    strcat(temp,t);
    char q[10];
    strcat(temp,"\nlongitude: ");
    sprintf(q,"%.4f",arr[i].lon);
    strcat(temp,q);
    strcat(temp,"\n");

    len = strlen(temp);
    if (offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, temp + offset, size);
    } else
        size = 0;

    return size;
}
static int postal_rename(const char *from, const char *to) {
	printf("rename | from: %s | to: %s\n",from,to);
	long int i = validate_path(from);
    if (i == -1) return -EPERM;
    if (strncmp(from,"/NAMES",6) == 0) {
    	char *n = getnname(to);
    	strcpy(arr[i].n,n);
    	free(n);
    }
    else if (strncmp(from,"/CODES",6) == 0) {
    	char *p = getpostcode(to);
    	strcpy(arr[i].code,p);
    	free(p);
    }
    else return -EPERM;
    
    writefile();
    return 0;
}

static int postal_unlink(const char *path)
{
	long int index = validate_path(path);
	printf("unlink | index: %ld\n",index);
    if (index == -1) return -EPERM;
    long int j;
    for(j=index; j < ncount-1;j++) { // shift array for filling the empty spot
    	arr[j] = arr[j+1];
    }
    ncount--;
    writefile();
    return 0;
}

static int postal_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *finfo)
{
    (void)path;
    (void)buf;
    (void)size;
    (void)offset;
    (void)finfo;
    return -EPERM;
}

static int postal_release(const char *path, struct fuse_file_info *finfo)
{
    (void) path;
    (void) finfo;
    return 0;
}

static int postal_fsync(const char *path, int crap, struct fuse_file_info *finfo)
{
    (void) path;
    (void) crap;
    (void) finfo;
    return 0;
}

static int postal_rmdir(const char *path)
{
    (void)path;
    return -EPERM;
}

static int postal_symlink(const char *from, const char *to)
{
    (void)from;
    (void)to;
    return -EPERM;
}

static int postal_link(const char *from, const char *to)
{
    (void)from;
    (void)to;
    return -EPERM;
}

static int postal_chmod(const char *path, mode_t mode)
{
    (void)path;
    (void)mode;
    return -EPERM;

}

static int postal_chown(const char *path, uid_t uid, gid_t gid)
{
    (void)path;
    (void)uid;
    (void)gid;
    return -EPERM;
}

static int postal_truncate(const char *path, off_t size)
{
    (void)path;
    (void)size;
    return -EPERM;
}

static int postal_utime(const char *path, struct utimbuf *buf)
{
    (void)path;
    (void)buf;
    return -EPERM;
}

static int postal_mknod(const char *path, mode_t mode, dev_t rdev)
{
    (void)path;
    (void)mode;
    (void)rdev;
    return -EPERM;
}

static int postal_mkdir(const char *path, mode_t mode)
{
    (void)path;
    (void)mode;
    return -EPERM;
}

static struct fuse_operations postal_oper = {
    .getattr	= postal_getattr,
    .readdir	= postal_readdir,
    .open	= postal_open,
    .read	= postal_read,
    .rename = postal_rename,
    .unlink = postal_unlink,
    .mknod       = postal_mknod,
    .mkdir       = postal_mkdir,
    .symlink     = postal_symlink,
    .rmdir       = postal_rmdir,
    .link        = postal_link,
    .chmod       = postal_chmod,
    .chown       = postal_chown,
    .truncate    = postal_truncate,
    .utime       = postal_utime,
    .write       = postal_write, 
    .release     = postal_release,
    .fsync       = postal_fsync, 
};


int main(int argc, char *argv[])
{
	char cwd[200];
	getcwd(cwd,sizeof(cwd)); // get current path
	strcpy(csvloc,cwd);
	strcat(csvloc,"/postal-codes.csv");
	struct stat ptr;
	lstat(csvloc,&ptr);
	mtime = ptr.st_mtime; // get last modification time of the file and write it to "mtime"
	printf("Postal file system is online.\n");
	readfile();
    return fuse_main(argc, argv, &postal_oper, NULL);
}
