#ifndef __FS_H_
#define __FS_H_

#define NUM_FILES 32
#define END 0x80000000
// #define FILE_SIZE 0x10// 16KB
#define FILE_SIZE 0x1000 // In Bytes


struct inode{
    int is_valid;
    char filename[256];
    u32 mode;
    u32 type;
    u32 inode_no;
    u32 ref_count;
    unsigned int s_pos;     //starting position of file
    unsigned int e_pos;     //ending position of file[ Max_File_Size]
    unsigned int max_pos;   // ending position of file data
    unsigned int file_size;  // file_size
    struct super_block *sb;

    int (*read) (struct inode *inode, char *buf, int count, int *offp);
    int (*write) (struct inode *inode, char *buf, int count, int *offp);
    int (*open) (struct inode *inode);
    int (*close) (struct inode *inode);

};

struct super_block{
    int num_files;
    char *fs_name;		//Name
    struct inode* inode[NUM_FILES];
    struct sb_operations *sb_op;

};


struct sb_operations{
    int (*remove_inode) (struct super_block *sb, struct inode *inode);
    int (*create_inode) (struct super_block *sb, char *filename, u32 mode);
    struct inode* (*lookup_inode) (struct super_block *sb, char *filename);
    int (*get_inode_no) ( struct super_block *sb, char *name);
    int (*get_num_files) (struct super_block *sb);
    void (*list_all_files) (struct super_block *sb, void *buf );
};

extern int flat_create_inode(struct super_block *sb, char *filename, u32 mode);
int flat_remove_inode(struct super_block *sb, struct inode *inode);
struct inode* flat_lookup_inode(struct super_block *sb, char *filename);
int flat_get_inode_no( struct super_block *sb, char *name);
int flat_get_num_files( struct super_block *sb);
void flat_list_all_files (struct super_block *sb, void *buf );


int flat_read(struct inode *inode, char *buf, int count, int *offp);
int flat_write(struct inode *inode, char *buf, int count, int *offp);
int flat_open(struct inode* inode);
int flat_close(struct inode *inode);


struct super_block * get_superblock();

extern void init_file_system();
extern struct inode* lookup_inode(char *filename);
extern struct inode *create_inode (char *filename, u64 mode);

#endif
