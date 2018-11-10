#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "ext2_structs.h"
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
