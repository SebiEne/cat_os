#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h>
#include <vfs.h>
#include <stdio.h>

/****************************************************************************
 * Private Functions Declaration
 ****************************************************************************/

int open_fs_node(void *priv, const char *pathname, int flags, mode_t mode);
int read_fs_node(void *priv, void *buf, size_t count);
int write_fs_node(void *priv, const void *buf, size_t count);
int close_fs_node(void *priv);

/****************************************************************************
 * Private Variables
 ****************************************************************************/

/* Fat file system instance */
static FATFS *g_fatfs;

/* Supported file system operations */
static struct vfs_ops_s g_fs_ops = {
  .open   = open_fs_node,
  .close  = close_fs_node,
  .read   = read_fs_node,
  .write  = write_fs_node,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/*
 * emit_vfs_node - creates a new node in the virtual file system for a FS object
 *
 * @mount_path - the readable location where we mount the file system
 * @name       - the file name with the path relative to this file system
 * @node_type  - the type of the VFS object (regular file or dir)
 *
 */
static int emit_vfs_node(const char *mount_path, const char *name,
  enum vfs_node_type node_type)
{
  size_t path_len = strlen(mount_path) + strlen(name) + 2;
  char *path = calloc(1, path_len);
  if (path == NULL) {
    return -ENOMEM;
  }

  snprintf(path, path_len, "/%s/%s", mount_path, name);
  int ret = vfs_register_node(path,
                              strlen(path),
                              &g_fs_ops,
                              node_type,
                              NULL);

  printf("Creating %s: %s status %d\n",
         node_type == VFS_TYPE_DIR ? "DIR" : "FILE", path, ret);
  return ret;
}

/*
 * scan_files - this method lists the internal file system contents
 *
 * @path - the readable location where we mount the file system
 *
 * WARNING: This method is recursive and it can blow the stack for deep
 *          directory structures. !!! FIXME
 */
static FRESULT scan_files(char *path)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;

    /* Open the directory */
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        i = strlen(path);
        for (;;) {

            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) break;
            if (fno.fname[0] == '.') continue;

            fn = fno.fname;

            if (fno.fattrib & AM_DIR) {
                sprintf(&path[i], "/%s", fn);
                emit_vfs_node("mnt", path, VFS_TYPE_DIR);

                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            } else {

              char *m_path = calloc(1, 80);
              if (m_path == NULL) {
                return res;
              }

              sprintf(m_path, "%s/%s", path, fn);
              emit_vfs_node("mnt", m_path, VFS_TYPE_FILE);
              free(m_path);
            }
        }
    }

    return res;
}

int open_fs_node(void *priv, const char *pathname, int flags, mode_t mode)
{
  return OK;
}

int read_fs_node(void *priv, void *buf, size_t count)
{
  return OK;
}

int write_fs_node(void *priv, const void *buf, size_t count)
{
  return OK;
}

int close_fs_node(void *priv)
{
  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int console_mount(int argc, const char *argv[])
{
  g_fatfs = malloc(sizeof(FATFS));
  if (g_fatfs == NULL) {
    printf("Error: FatFS not initialized, not enough mem\n");
    return -ENOMEM;
  } else {
    FRESULT ret = f_mount(g_fatfs, "", 1);
    if (ret != FR_OK) {
      printf("Error: %d cannot mount FatFS\n", ret);
      free(g_fatfs);
      return -ENOSYS;
    }
  }

  char *path = malloc(256);
  memset(path, 0, 256);
  strncpy(path, "/", 1);

  printf("Filesystem mounted.\n");

  scan_files(path);
  free(path);

  return OK;
}

int console_umount(int argc, const char *argv[]) {
  if (g_fatfs == NULL) {
    printf("There is no FS mounted\n");
  }

  f_mount(NULL, "", 0);
  free(g_fatfs);
  g_fatfs = NULL;
}