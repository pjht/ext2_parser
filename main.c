#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
typedef struct {
  uint32_t s_inodes_count;
  uint32_t s_blocks_count;
  uint32_t s_r_blocks_count;
  uint32_t s_free_blocks_count;
  uint32_t s_free_inodes_count;
  uint32_t s_first_data_block;
  uint32_t s_log_blk_size;
  uint32_t s_log_frag_size;
  uint32_t s_blocks_per_group;
  uint32_t s_frags_per_group;
  uint32_t s_inodes_per_group;
  uint32_t s_mtime;
  uint32_t s_wtime;
  uint16_t s_mnt_count;
  uint16_t s_max_mnt_count;
  uint16_t s_magic;
  uint16_t s_state;
  uint16_t s_errors;
  uint16_t s_minor_rev_level;
  uint32_t s_lastcheck;
  uint32_t s_checkinterval;
  uint32_t s_creator_os;
  uint32_t s_rev_level;
  uint16_t s_def_resuid;
  uint16_t s_def_resgid;
  uint32_t s_first_ino;
  uint16_t s_inode_size;
  uint16_t s_block_group_nr;
  uint32_t s_feature_compat;
  uint32_t s_feature_incompat;
  uint32_t s_feature_ro_compat;
  char s_uuid[16];
  char s_volume_name[16];
  char s_last_mounted[64];
  uint32_t s_algo_bitmap;
  char s_prealloc_blocks;
  char s_prealloc_dir_blocks;
  char s_journal_uuid[16];
  uint32_t s_journal_inum;
  uint32_t s_journal_dev;
  uint32_t s_last_orphan;
} __attribute__((packed)) ext2_superblock;

typedef enum {
  EXT2_FEATURE_COMPAT_DIR_PREALLOC=1,
  EXT2_FEATURE_COMPAT_IMAGIC_INODES=2,
  EXT2_FEATURE_COMPAT_HAS_JOURNAL=4,
  EXT2_FEATURE_COMPAT_EXT_ATTR=8,
  EXT2_FEATURE_COMPAT_RESIZE_INO=16,
  EXT2_FEATURE_COMPAT_DIR_INDEX=32
}  s_feature_compat;

typedef enum {
  EXT2_FEATURE_INCOMPAT_COMPRESSION=1,
  EXT2_FEATURE_INCOMPAT_FILETYPE=2,
  EXT2_FEATURE_INCOMPAT_RECOVER=4,
  EXT2_FEATURE_INCOMPAT_JOURNAL_DEV=8
}  s_feature_incompat;

typedef enum {
  EXT2_FEATURE_RO_COMPAT_SPARSE_SUPER=1,
  EXT2_FEATURE_RO_COMPAT_LARGE_FILE=2,
  EXT2_FEATURE_RO_COMPAT_BTREE_DIR=4
}  s_feature_ro_compat;

typedef struct {
  uint32_t bg_blk_bitmap;
  uint32_t bg_inode_bitmap;
  uint32_t bg_inode_table;
  uint16_t bg_free_blocks_count;
  uint16_t bg_free_inodes_count;
  uint16_t bg_used_dirs_count;
  char unused[14];
} __attribute__((packed)) blk_grp;

typedef struct {
  uint16_t i_mode;
  uint16_t i_uid;
  uint32_t i_size;
  uint32_t i_atime;
  uint32_t i_ctime;
  uint32_t i_mtime;
  uint32_t i_dtime;
  uint16_t i_gid;
  uint16_t i_links_count;
  uint32_t i_blocks;
  uint32_t i_flags;
  uint32_t i_osd1;
  uint32_t i_block[15];
  uint32_t i_generation;
  uint32_t i_file_acl;
  uint32_t i_dir_acl;
  uint32_t i_faddr;
  uint32_t i_osd2;
  char unused[8];
} __attribute__((packed)) inode;

typedef enum {
  EXT2_S_IFIFO=0x100,
  EXT2_S_IFCHR=0x2000,
  EXT2_S_IFDIR=0x4000,
  EXT2_S_IFBLK=0x6000,
  EXT2_S_IFREG=0x8000,
  EXT2_S_IFLNK=0xA000,
  EXT2_S_IFSOCK=0xC000
} i_mode;

typedef enum {
  EXT2_SECRM_FL=0x1,
  EXT2_UNRM_FL=0x2,
  EXT2_COMPR_FL=0x4,
  EXT2_SYNC_FL=0x8,
  EXT2_IMMUTABLE_FL=0x10,
  EXT2_APPEND_FL=0x20,
  EXT2_NODUMP_FL=0x40,
  EXT2_NOATIME_FL=0x80,
  EXT2_DIRTY_FL=0x100,
  EXT2_COMPRBLK_FL=0x200,
  EXT2_NOCOMPR_FL=0x400,
  EXT2_ECOMPR_FL=0x800,
  EXT2_BTREE_FL=0x1000,
  EXT2_INDEX_FL=0x1000,
  EXT2_IMAGIC_FL=0x2000,
  EXT2_JOURNAL_DATA_FL=0x4000,
  EXT2_RESERVED_FL=0x80000000
} i_flags;

typedef struct {
  uint32_t inode;
  uint16_t rec_len;
  char name_len;
  char file_type;
} __attribute__((packed)) dir_entry;

char img[33554432];
ext2_superblock* supblk;
uint32_t blk_size;
blk_grp** blk_grps;

void show_supblk_info(int num_blk_grps) {
  printf("Num block groups:%d\n",num_blk_grps);
  printf("Number of blocks per group:%d\n",supblk->s_blocks_per_group);
  printf("Number of inodes per group:%d\n",supblk->s_inodes_per_group);
  printf("Number of unallocated blocks:%d\n",supblk->s_free_blocks_count);
  printf("Number of unallocated inodes:%d\n",supblk->s_free_inodes_count);
  printf("Optional features:");
  if ((supblk->s_feature_compat&EXT2_FEATURE_COMPAT_DIR_PREALLOC)!=0) {
    printf("DIR_PREALLOC ");
  }
  if ((supblk->s_feature_compat&EXT2_FEATURE_COMPAT_IMAGIC_INODES)!=0) {
    printf("IMAGIC_INODES ");
  }
  if ((supblk->s_feature_compat&EXT2_FEATURE_COMPAT_HAS_JOURNAL)!=0) {
    printf("HAS_JOURNAL ");
  }
  if ((supblk->s_feature_compat&EXT2_FEATURE_COMPAT_EXT_ATTR)!=0) {
    printf("EXT_ATTR ");
  }
  if ((supblk->s_feature_compat&EXT2_FEATURE_COMPAT_RESIZE_INO)!=0) {
    printf("RESIZE_INO ");
  }
  if ((supblk->s_feature_compat&EXT2_FEATURE_COMPAT_DIR_INDEX)!=0) {
    printf("DIR_INDEX ");
  }
  printf("\n");
  printf("Required features:");
  if ((supblk->s_feature_incompat&EXT2_FEATURE_INCOMPAT_COMPRESSION)!=0) {
    printf("COMPRESSION ");
  }
  if ((supblk->s_feature_incompat&EXT2_FEATURE_INCOMPAT_FILETYPE)!=0) {
    printf("FILETYPE ");
  }
  if ((supblk->s_feature_incompat&EXT2_FEATURE_INCOMPAT_RECOVER)!=0) {
    printf("RECOVER ");
  }
  if ((supblk->s_feature_incompat&EXT2_FEATURE_INCOMPAT_JOURNAL_DEV)!=0) {
    printf("JOURNAL_DEV ");
  }
  printf("\n");
  printf("Readonly features:");
  if ((supblk->s_feature_ro_compat&EXT2_FEATURE_RO_COMPAT_SPARSE_SUPER)!=0) {
    printf("SPARSE_SUPER ");
  }
  if ((supblk->s_feature_ro_compat&EXT2_FEATURE_RO_COMPAT_LARGE_FILE)!=0) {
    printf("LARGE_FILE ");
  }
  if ((supblk->s_feature_ro_compat&EXT2_FEATURE_RO_COMPAT_BTREE_DIR)!=0) {
    printf("BTREE_DIR ");
  }
  printf("\n");
}

void show_inode_info(inode inod) {
  printf("Flags:");
  if ((inod.i_flags&EXT2_SECRM_FL)!=0) {
    printf("SECRM_FL");
  }
  if ((inod.i_flags&EXT2_UNRM_FL)!=0) {
    printf("UNRM_FL");
  }
  if ((inod.i_flags&EXT2_COMPR_FL)!=0) {
    printf("COMPR_FL");
  }
  if ((inod.i_flags&EXT2_SYNC_FL)!=0) {
    printf("SYNC_FL");
  }
  if ((inod.i_flags&EXT2_IMMUTABLE_FL)!=0) {
    printf("IMMUTABLE_FL");
  }
  if ((inod.i_flags&EXT2_APPEND_FL)!=0) {
    printf("APPEND_FL");
  }
  if ((inod.i_flags&EXT2_NODUMP_FL)!=0) {
    printf("NODUMP_FL");
  }
  if ((inod.i_flags&EXT2_NOATIME_FL)!=0) {
    printf("NOATIME_FL");
  }
  if ((inod.i_flags&EXT2_DIRTY_FL)!=0) {
    printf("DIRTY_FL");
  }
  if ((inod.i_flags&EXT2_COMPRBLK_FL)!=0) {
    printf("COMPRBLK_FL");
  }
  if ((inod.i_flags&EXT2_NOCOMPR_FL)!=0) {
    printf("NOCOMPR_FL");
  }
  if ((inod.i_flags&EXT2_ECOMPR_FL)!=0) {
    printf("ECOMPR_FL");
  }
  if ((inod.i_flags&EXT2_BTREE_FL)!=0) {
    printf("BTREE_FL");
  }
  if ((inod.i_flags&EXT2_INDEX_FL)!=0) {
    printf("INDEX_FL");
  }
  if ((inod.i_flags&EXT2_IMAGIC_FL)!=0) {
    printf("IMAGIC_FL");
  }
  if ((inod.i_flags&EXT2_JOURNAL_DATA_FL)!=0) {
    printf("JOURNAL_DATA_FL");
  }
  if ((inod.i_flags&EXT2_RESERVED_FL)!=0) {
    printf("RESERVED_FL");
  }
  printf("\n");
}

void* read_blk(blknum) {
  return (void*)(&img[blknum*1024]);
}

inode read_inode(uint32_t inode_num) {
  uint32_t grp=(inode_num-1)/supblk->s_inodes_per_group;
  uint32_t index=(inode_num-1)%supblk->s_inodes_per_group;
  uint32_t starting_blk=blk_grps[grp]->bg_inode_table;
  inode* inodes=read_blk(starting_blk);
  return inodes[index];
}

int main() {
  FILE* f=fopen("ext2.img","rb");
  if (f) {
    fread(img,1,32*1024*1024,f);
    fclose(f);
  }
  supblk=read_blk(1);
  blk_size=1024<<(supblk->s_log_blk_size);
  double num_blk_grps_dbl=supblk->s_blocks_count/(double)supblk->s_blocks_per_group;
  uint32_t num_blk_grps=ceil(num_blk_grps_dbl);
  show_supblk_info(num_blk_grps);
  blk_grps=malloc(sizeof(blk_grp*)*num_blk_grps);
  blk_grp* blk_group=read_blk(2);
  for (int i=0;i<num_blk_grps;i++) {
    printf("Block group %d:\n",i);
    printf("Number of unallocated blocks:%d\n",blk_group->bg_free_blocks_count);
    printf("Number of unallocated inodes:%d\n",blk_group->bg_free_inodes_count);
    blk_grps[i]=blk_group;
    blk_group++;
  }
  printf("root dir:\n");
  inode root_dir_inode=read_inode(2);
  show_inode_info(root_dir_inode);
  dir_entry* root_dir=read_blk(root_dir_inode.i_block[0]);
  dir_entry* current_entry=root_dir;
  for(int i=0;;i++) {
    if (current_entry->file_type==0) {
      break;
    }
    printf("Directory entry %d:\n",i);
    printf("Directory entry inode:%d\n",current_entry->inode);
    printf("Length of entry:%d\n",current_entry->rec_len);
    printf("Length of name:%d\n",current_entry->name_len);
    printf("File type:%d\n",current_entry->file_type);
    printf("File name:%s\n",(char*)current_entry+sizeof(dir_entry));
    current_entry=(dir_entry*)(((uint64_t)current_entry)+current_entry->rec_len);
  }
  printf("lost+found:\n");
  inode lost_dir_inode=read_inode(11);
  show_inode_info(lost_dir_inode);
  printf("Block:%d\n",lost_dir_inode.i_block[0]);
  dir_entry* lost_dir=read_blk(lost_dir_inode.i_block[0]);
  current_entry=lost_dir;
  for(int i=0;;i++) {
    if (current_entry->file_type==0) {
      break;
    }
    printf("Directory entry %d:\n",i);
    printf("Directory entry inode:%d\n",current_entry->inode);
    printf("Length of entry:%d\n",current_entry->rec_len);
    printf("Length of name:%d\n",current_entry->name_len);
    printf("File type:%d\n",current_entry->file_type);
    printf("File name:%s\n",(char*)current_entry+sizeof(dir_entry));
    current_entry=(dir_entry*)(((uint64_t)current_entry)+current_entry->rec_len);
  }
}
