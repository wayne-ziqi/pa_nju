#include "common.h"
#include "string.h"

typedef struct
{
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

typedef struct
{
	bool used;
	uint32_t index;
	uint32_t offset;
} Fstate;

enum
{
	SEEK_SET, //设置光标
	SEEK_CUR, //当前位置+=
	SEEK_END  //文件末尾+=
};

static const file_info file_table[] = {
	{"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440}, {"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168}, {"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896}, {"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164}, {"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609}, {"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401}, {"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689}, {"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709}, {"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225}, {"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915}, {"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993}, {"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411}, {"word.dat", 5650, 28155717}, {"credits_bgm.wav", 6631340, 1048576},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

Fstate files[NR_FILES + 3]; //前三个文件对应stdin, stdout, stderr

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

/* Please implement a simple file system here */

int fs_open(const char *pathname, int flags)
{
    //遍历file_table找有没有当前文件，有的话把相应fstate的offset置为0（从文件头开始读写），index置为file_info的对应项
	//panic("Please implement fs_open at fs.c");
	//Log("call fs_open\n");
	for(int i = 0;i<NR_FILES;i++){
	    if(strcmp(file_table[i].name, pathname) == 0){
	        files[i+3].index = i + 3;
	        files[i+3].used = 1;
	        files[i+3].offset = 0;
	        return files[i+3].index;
	    }
	}
    Log("File %s doesn't exist\n", pathname);
    assert(0);
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
    //根据fd索引到文件，判断：当前文件offset超过文件大小返回，读取长度不能越界否则截取，读入buf并移动指针。
	assert(fd > 2 && fd < NR_FILES + 3);
	//Log("fs_read file %s\n", file_table[fd-3].name);
	Fstate currFile = files[fd];
	assert(currFile.used == 1);
	if(currFile.offset >= file_table[fd - 3].size)return 0;
	int rdlen = len;
	if(currFile.offset + len >= file_table[fd-3].size) 
	    rdlen = file_table[fd - 3].size - currFile.offset - 1;
	memset(buf, 0, len); //必须要把buf的清零，否则出现fp!=0的assertion
	//buf[rdlen] = '\0';
	ide_read(buf, file_table[fd - 3].disk_offset + currFile.offset, rdlen);
	files[fd].offset += rdlen;
	//Log("load from ide done\n");
	return rdlen;
}

size_t fs_write(int fd, void *buf, size_t len)
{
	assert(fd <= 2 && fd >= 0);
#ifdef HAS_DEVICE_SERIAL
	int i;
	extern void serial_printc(char);
	for (i = 0; i < len; i++)
	{
		serial_printc(((char *)buf)[i]);
	}
#else
	asm volatile(".byte 0x82"
				 : "=a"(len)
				 : "a"(4), "b"(fd), "c"(buf), "d"(len));
#endif

	return len;
}

off_t fs_lseek(int fd, off_t offset, int whence)
{
	//panic("Please implement fs_lseek at fs.c");
	//Log("call fs_lseek\n");
	assert(fd >= 0 && fd < NR_FILES + 3);
	switch(whence){
	    case SEEK_SET:{
	        if(offset >= file_table[fd - 3].size)files[fd].offset = file_table[fd - 3].size - 1;
	        else files[fd].offset = offset;
	        return files[fd].offset;
	    }
	    case SEEK_CUR:{
	        if(files[fd].offset + offset >= file_table[fd-3].size)files[fd].offset = file_table[fd - 3].size - 1;
	        else files[fd].offset += offset;
	        return files[fd].offset;
	    }
	    case SEEK_END:{
	        assert (fd > 2 && offset <= 0);//系统文件不可编辑
	        files[fd].offset = file_table[fd-3].size  + offset; //-1
	        return files[fd].offset;
	    }
	}
	Log("illegal whence %d\n", whence);
	assert(0);
	return -1;
}

int fs_close(int fd)
{
	//panic("Please implement fs_close at fs.c");
	//Log("call fs_close\n");
	assert(fd >=0 &&fd < NR_FILES + 3);
	files[fd].used = 0;
	return 0;
}
