#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/soc/qcom/smem.h>
#include <linux/seq_file.h>
#include <linux/platform_device.h>
#include <soc/oplus/system/oplus_project.h>
#include <linux/io.h>
#include <stdbool.h>

#include "oplus_nfc.h"

#define NFC_CHIPSET_VERSION (0x1)

static char current_chipset[32];
static bool support_nfc = false;

bool is_nfc_support()
{
	return support_nfc;
}

bool is_support_chip(chip_type chip)
{
	bool ret = false;
	const char* target_chipset;

	if (!support_nfc)
	{
		pr_err("%s, nfc not supported, or oplus_nfc has not started", __func__);
		return false;
	}

	switch(chip) {
		case NQ310:
			target_chipset = "NQ310|NQ330|PN557";
			break;
		case NQ330:
			target_chipset = "NQ330";
			break;
		case SN100T:
			target_chipset = "SN100T|SN110T|SN220P|PN560";
			break;
		case SN100F:
			target_chipset = "SN100F";
			break;
		case SN110T:
			target_chipset = "SN100T|SN110T";
			break;
		case ST21H:
			target_chipset = "ST21H|ST54H";
			break;
		case ST54H:
			target_chipset = "ST54H";
			break;
                case THN31:
			target_chipset = "THN31";
			break;
		default:
			target_chipset = "UNKNOWN";
			break;
	}

	if (strstr(target_chipset, current_chipset) != NULL)
	{
		ret = true;
	}

	pr_err("oplus_nfc target_chipset = %s, current_chipset = %s \n", target_chipset, current_chipset);
	return ret;
}
EXPORT_SYMBOL(is_support_chip);

static int nfc_read_func(struct seq_file *s, void *v)
{
	void *p = s->private;

	switch((unsigned long)(p)) {
	case NFC_CHIPSET_VERSION:
		seq_printf(s, "%s", current_chipset);
		break;
	default:
		seq_printf(s, "not support\n");
		break;
	}

	return 0;
}

static int nfc_open(struct inode *inode, struct file *file)
{
	return single_open(file, nfc_read_func, PDE_DATA(inode));
}

static const struct proc_ops nfc_info_fops = {
	.proc_open  = nfc_open,
	.proc_read  = seq_read,
        .proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int oplus_nfc_probe(struct platform_device *pdev)
{
	struct device* dev;
	unsigned int project = 0;
	int operator = 0;
	int readRet = -1;
	char prop_project[32];
	char prop_project_operator[32];
	const char *chipset_node;
        const char *chipset_manifest;
	struct proc_dir_entry *p_entry;
	static struct proc_dir_entry *nfc_info = NULL;

	pr_err("enter %s", __func__);
        memset(prop_project, 0, sizeof(prop_project));
        memset(prop_project_operator, 0, sizeof(prop_project_operator));

	dev = &pdev->dev;
	if (!dev)
	{
		pr_err("%s, no device", __func__);
		goto error_init;
	}
	project = get_project();
	operator = get_Operator_Version();

	//project name consists of 5-symbol
	//project contains letters is big then 0x10000 == 65536
	if (project > 0x10000)
                snprintf(prop_project, sizeof(prop_project) , "chipset-%X", project);
	else
                snprintf(prop_project, sizeof(prop_project) , "chipset-%u", project);

	/*
	 *fixed me
	 *if operator is valid
	 *dts still uses by prop_project? not by prop_project_operator
	 */
	if (operator >= 0) {
                snprintf(prop_project_operator, sizeof(prop_project_operator) , "%s-%d", prop_project, operator);
	        pr_err("%s, prop_project_operator to be read = %s", __func__, prop_project_operator);
	        readRet = of_property_read_string(dev->of_node, prop_project_operator, &chipset_node);
	}

	if (readRet != 0){
                pr_err("%s, prop_project to be read = %s", __func__, prop_project);
                readRet = of_property_read_string(dev->of_node, prop_project, &chipset_node);
	}

	if (readRet != 0)
	{
		snprintf(current_chipset, sizeof(current_chipset) , "NULL");
	} else
	{
		pr_err("%s, get chipset_node content = %s", __func__, chipset_node);
		strncpy(current_chipset, chipset_node, sizeof(current_chipset) - 1);
		support_nfc = true;
	}

	nfc_info = proc_mkdir("oplus_nfc", NULL);
	if (!nfc_info)
	{
		pr_err("%s, make oplus_nfc dir fail", __func__);
		goto error_init;
	}

	p_entry = proc_create_data("chipset", S_IRUGO, nfc_info, &nfc_info_fops, (unsigned long *)(NFC_CHIPSET_VERSION));
	if (!p_entry)
	{
		pr_err("%s, make chipset node fail", __func__);
		goto error_init;
	}

        if (of_property_read_string(dev->of_node, "manifest", &chipset_manifest)) {
                pr_err("read manifest node fail");
        } else {
	        pr_err("%s, get chipset_node content = %s", __func__, chipset_manifest);
                pr_err("read manifest node success");
                proc_symlink("manifest", nfc_info , chipset_manifest);
        }

	return 0;

error_init:
	pr_err("%s error_init", __func__);
	remove_proc_entry("oplus_nfc", NULL);
	return -ENOENT;
}

static int oplus_nfc_remove(struct platform_device *pdev)
{
	remove_proc_entry("oplus_nfc", NULL);
	return 0;
}

static const struct of_device_id onc[] = {
	{.compatible = "oplus-nfc-chipset", },
	{},
};

MODULE_DEVICE_TABLE(of, onc);

static struct platform_driver oplus_nfc_driver = {
	.probe  = oplus_nfc_probe,
	.remove = oplus_nfc_remove,
	.driver = {
		.name = "oplus-nfc-chipset",
		.of_match_table = of_match_ptr(onc),
	},
};

static int __init oplus_nfc_init(void)
{
	pr_err("enter %s", __func__);
	return platform_driver_register(&oplus_nfc_driver);
}

subsys_initcall(oplus_nfc_init);

static void __exit oplus_nfc_exit(void)
{
	platform_driver_unregister(&oplus_nfc_driver);
}
module_exit(oplus_nfc_exit);

MODULE_DESCRIPTION("OPLUS nfc chipset version");
MODULE_LICENSE("GPL v2");
