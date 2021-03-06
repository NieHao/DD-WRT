/*
 * Sierra Wireless CDMA Wireless Serial USB drive
 * 
 * Current Copy modified by: Kevin Lloyd <linux@sierrawireless.com>
 * Original Copy written by: 2005 Greg Kroah-Hartman <gregkh <at> suse.de>
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License version
 *	2 as published by the Free Software Foundation.
 *
 * Version history:
  Version 1.03 (Lloyd):
  Included support for DTR control and enhanced buffering (should help
  speed).
 */

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/smp_lock.h>

#ifdef CONFIG_USB_SERIAL_DEBUG
	static int debug = 1;
#else
	static int debug;
#endif

#include "usb-serial.h"
#include "sierra.h"
#if defined(CONFIG_USB_SERIAL_PL2303) || defined(CONFIG_USB_SERIAL_PL2303_MODULE)
#include "pl2303.h"  // see /* BEGIN HORRIBLE HACK FOR PL2303 */ below
#endif

#define DRIVER_VERSION "v1.03"

#if 0
#define USB_VENDER_REQUEST_SET_DEVICE_POWER_STATE 0

#define USB_DEVICE_POWER_STATE_D0	0x0000
#define USB_DEVICE_POWER_STATE_D1	0x0001
#define USB_DEVICE_POWER_STATE_D2	0x0002
#define USB_DEVICE_POWER_STATE_D3	0x0003

#define SET_CONTROL_LINE_STATE		0x22
/*
 * Output control lines.
 */

#define ACM_CTRL_DTR		0x01
#define ACM_CTRL_RTS		0x02
#endif

//static int sw_attach(struct usb_serial *serial);
static void sw_usb_serial_generic_shutdown(struct usb_serial *serial);

static int  sw_usb_serial_generic_open(struct usb_serial_port *port, struct file *filp);
static void sw_usb_serial_generic_close(struct usb_serial_port *port, struct file *filp);

#if 1
/*-----------------------------------------------------------*/
static int                      serial_refcount;
static struct tty_driver        serial_tty_driver;
static struct tty_struct *      serial_tty[SERIAL_TTY_MINORS];
static struct termios *         serial_termios[SERIAL_TTY_MINORS];
static struct termios *         serial_termios_locked[SERIAL_TTY_MINORS];
static struct usb_serial        *serial_table[SERIAL_TTY_MINORS];       /* initially all NULL */

static LIST_HEAD(usb_serial_driver_list);
static struct usb_serial *get_free_serial (int num_ports, int *minor);
static void generic_write_bulk_callback (struct urb *urb);
static void * usb_serial_probe(struct usb_device *dev, unsigned int ifnum,const struct usb_device_id *id);
static void usb_serial_disconnect(struct usb_device *dev, void *ptr);
static void sw_usb_serial_generic_read_bulk_callback (struct urb *urb);
static void port_softint(void *private);
static void return_serial (struct usb_serial *serial);
static struct usb_serial *get_free_serial (int num_ports, int *minor);
static int serial_read_proc (char *page, char **start, off_t off, int count, int *eof, void *data);
/*-----------------------------------------------------------*/

#endif


static struct usb_device_id id_table [] = {
	{ USB_DEVICE(0x1199, 0x0017) },	/* Sierra Wireless EM5625 */
	{ USB_DEVICE(0x1199, 0x0018) },	/* Sierra Wireless MC5720 */
	{ USB_DEVICE(0x1199, 0x0218) },	/* Sierra Wireless MC5720 */
	{ USB_DEVICE(0x0f30, 0x1b1d) },	/* Sierra Wireless MC5720 */
	{ USB_DEVICE(0x1199, 0x0020) },	/* Sierra Wireless MC5725 */
	{ USB_DEVICE(0x1199, 0x0220) },	/* Sierra Wireless MC5725 */
	{ USB_DEVICE(0x1199, 0x0019) },	/* Sierra Wireless AirCard 595 */
	{ USB_DEVICE(0x1199, 0x0021) },	/* Sierra Wireless AirCard 597E */
	{ USB_DEVICE(0x1199, 0x0120) },	/* Sierra Wireless USB Dongle 595U */
	{ USB_DEVICE(0x1199, 0x0023) },	/* Sierra Wireless C597 */

	{ USB_DEVICE(0x1199, 0x6802) },	/* Sierra Wireless MC8755 */
	{ USB_DEVICE(0x1199, 0x6804) },	/* Sierra Wireless MC8755 */
	{ USB_DEVICE(0x1199, 0x6803) },	/* Sierra Wireless MC8765 */
	{ USB_DEVICE(0x1199, 0x6812) },	/* Sierra Wireless MC8775 & AC 875U */
	{ USB_DEVICE(0x1199, 0x6813) },	/* Sierra Wireless MC8775 (Thinkpad internal) */
	{ USB_DEVICE(0x1199, 0x6815) }, /* Sierra Wireless MC8775 */
	{ USB_DEVICE(0x03f0, 0x1e1d) }, /* HP hs2300 a.k.a MC8775 */
	{ USB_DEVICE(0x1199, 0x6820) },	/* Sierra Wireless AirCard 875 */
	{ USB_DEVICE(0x1199, 0x6821) },	/* Sierra Wireless AirCard 875U */
	{ USB_DEVICE(0x1199, 0x6832) },	/* Sierra Wireless MC8780*/
	{ USB_DEVICE(0x1199, 0x6833) },	/* Sierra Wireless MC8781*/
	{ USB_DEVICE(0x1199, 0x683B) },	/* Sierra Wireless MC8785 Composite*/
	{ USB_DEVICE(0x1199, 0x6850) },	/* Sierra Wireless AirCard 880 */
	{ USB_DEVICE(0x1199, 0x6851) },	/* Sierra Wireless AirCard 881 */
	{ USB_DEVICE(0x1199, 0x6852) },	/* Sierra Wireless AirCard 880 E */
	{ USB_DEVICE(0x1199, 0x6853) },	/* Sierra Wireless AirCard 881 E */
	{ USB_DEVICE(0x1199, 0x6855) },	/* Sierra Wireless AirCard 880 U */
	{ USB_DEVICE(0x1199, 0x6856) },	/* Sierra Wireless AirCard 881 U */
	{ USB_DEVICE(0x1199, 0x6859) },	/* Sierra Wireless AirCard 885 E */
	{ USB_DEVICE(0x1199, 0x685A) },	/* Sierra Wireless AirCard 885 E */

	/* Sierra Wireless C885 */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1199, 0x6880, 0xFF, 0xFF, 0xFF)},
	/* Sierra Wireless Device */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1199, 0x6890, 0xFF, 0xFF, 0xFF)},
	/* Sierra Wireless Device */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1199, 0x6891, 0xFF, 0xFF, 0xFF)},
	/* Sierra Wireless Device */
	{ USB_DEVICE_AND_INTERFACE_INFO(0x1199, 0x6892, 0xFF, 0xFF, 0xFF)},

	{ USB_DEVICE(0x1199, 0x6468) }, /* Sierra Wireless MP3G - EVDO */
	{ USB_DEVICE(0x1199, 0x6469) }, /* Sierra Wireless MP3G - UMTS/HSPA */

	{ USB_DEVICE(0x1199, 0x0112) }, /* Sierra Wireless AirCard 580 */
	{ USB_DEVICE(0x0F3D, 0x0112) }, /* AirPrime/Sierra Wireless PC 5220 */
	{ USB_DEVICE(0x05C6, 0x6613) }, /* Onda H600/ZTE MF330 */
	{ }
};

MODULE_DEVICE_TABLE(usb, id_table);

static struct usb_driver sierra_driver = {
//	.owner =	THIS_MODULE,
	.name =		"Sierra wireless",
	.probe =	usb_serial_probe,
	.disconnect =	usb_serial_disconnect,
	.id_table =	id_table,
};

static struct usb_serial_device_type sierra_device = {
//	.driver = {
	.owner =		THIS_MODULE,
	.name =			"Sierra Wireless",
//	},
	.id_table =		id_table,
	.num_interrupt_in =	NUM_DONT_CARE,
	.num_bulk_in =		NUM_DONT_CARE,
	.num_bulk_out =		NUM_DONT_CARE,
	.num_ports =		3,
	//.startup =		sw_attach,
	.shutdown =		sw_usb_serial_generic_shutdown,
	.open = 		sw_usb_serial_generic_open,
	.close = 		sw_usb_serial_generic_close,
};

#define MAX_NUM_PORTS 8

/* local function prototypes */
static int  serial_open (struct tty_struct *tty, struct file * filp);
static void serial_close (struct tty_struct *tty, struct file * filp);
static int  serial_write (struct tty_struct * tty, int from_user, const unsigned char *buf, int count);
static int  serial_write_room (struct tty_struct *tty);
static int  serial_chars_in_buffer (struct tty_struct *tty);
static void serial_throttle (struct tty_struct * tty);
static void serial_unthrottle (struct tty_struct * tty);
static int  serial_ioctl (struct tty_struct *tty, struct file * file, unsigned int cmd, unsigned long arg);
static void serial_set_termios (struct tty_struct *tty, struct termios * old);
//static void serial_shutdown (struct usb_serial *serial);
static void serial_break (struct tty_struct *tty, int break_state);
static int generic_write (struct usb_serial_port *port, int from_user, const unsigned char *buf, int count);
static int generic_write_room (struct usb_serial_port *port);
static void generic_cleanup (struct usb_serial_port *port);
static int generic_chars_in_buffer (struct usb_serial_port *port);
//static void generic_shutdown (struct usb_serial *serial);

#if 1
static struct tty_driver serial_tty_driver = {
        .magic =                TTY_DRIVER_MAGIC,
        .driver_name =          "usb-serial",
#ifndef CONFIG_DEVFS_FS
        .name =                 "ttyUSB",
#else
        .name =                 "usb/tts/%d",
#endif
        .major =                SERIAL_TTY_MAJOR,
        .minor_start =          0,
        .num =                  SERIAL_TTY_MINORS,
        .type =                 TTY_DRIVER_TYPE_SERIAL,
        .subtype =              SERIAL_TYPE_NORMAL,
        .flags =                TTY_DRIVER_REAL_RAW | TTY_DRIVER_NO_DEVFS,

        .refcount =             &serial_refcount,
        .table =                serial_tty,
        .termios =              serial_termios,
        .termios_locked =       serial_termios_locked,

        .open =                 serial_open,
        .close =                serial_close,
        .write =                serial_write,
        .write_room =           serial_write_room,
        .ioctl =                serial_ioctl,
        .set_termios =          serial_set_termios,
        .throttle =             serial_throttle,
        .unthrottle =           serial_unthrottle,
        .break_ctl =            serial_break,
        .chars_in_buffer =      serial_chars_in_buffer,
        .read_proc =            serial_read_proc,
};
#endif


/*****************************************************************************
 * Driver tty interface functions
 *****************************************************************************/
static struct usb_serial *get_serial_by_minor (unsigned int minor)
{
        return serial_table[minor];
}

static int serial_open (struct tty_struct *tty, struct file * filp)
{
	struct usb_serial *serial;
	struct usb_serial_port *port;
	unsigned int portNumber;
	int retval = 0;
	
	dbg("%s", __FUNCTION__);

	/* initialize the pointer incase something fails */
	tty->driver_data = NULL;

	/* get the serial object associated with this tty pointer */
	serial = get_serial_by_minor (MINOR(tty->device));

	if (serial_paranoia_check (serial, __FUNCTION__))
		return -ENODEV;

	/* set up our port structure making the tty driver remember our port object, and us it */
	portNumber = MINOR(tty->device) - serial->minor;
	port = &serial->port[portNumber];
	tty->driver_data = port;

	down (&port->sem);
	port->tty = tty;
	 
	/* lock this module before we call it */
	if (serial->type->owner)
		__MOD_INC_USE_COUNT(serial->type->owner);

	++port->open_count;
	if (port->open_count == 1) {
		/* only call the device specific open if this 
		 * is the first time the port is opened */
		if (serial->type->open)
			retval = serial->type->open(port, filp);
		else
			retval = sw_usb_serial_generic_open(port, filp);//@.@
	}

	if (retval) {
		port->open_count = 0;
		if (serial->type->owner)
			__MOD_DEC_USE_COUNT(serial->type->owner);
	}

	up (&port->sem);
	return retval;
}

static void __serial_close(struct usb_serial_port *port, struct file *filp)
{
	if (!port->open_count) {
		dbg ("%s - port not opened", __FUNCTION__);
		return;
	}

	--port->open_count;
	if (port->open_count <= 0) {
		/* only call the device specific close if this 
		 * port is being closed by the last owner */
		if (port->serial->type->close)
			port->serial->type->close(port, filp);
		else
			//generic_close(port, filp);
			sw_usb_serial_generic_close(port, filp);
		port->open_count = 0;
	}

	if (port->serial->type->owner)
		__MOD_DEC_USE_COUNT(port->serial->type->owner);
}

static void serial_close(struct tty_struct *tty, struct file * filp)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial)
		return;

	down (&port->sem);

	dbg("%s - port %d", __FUNCTION__, port->number);

	/* if disconnect beat us to the punch here, there's nothing to do */
	if (tty->driver_data) {
		__serial_close(port, filp);
	}

	up (&port->sem);
}

static int serial_write (struct tty_struct * tty, int from_user, const unsigned char *buf, int count)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	int retval = -EINVAL;

	if (!serial)
		return -ENODEV;

	down (&port->sem);

	dbg("%s - port %d, %d byte(s)", __FUNCTION__, port->number, count);

	if (!port->open_count) {
		dbg("%s - port not opened", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (serial->type->write)
		retval = serial->type->write(port, from_user, buf, count);
	else
		retval = generic_write(port, from_user, buf, count);

exit:
	up (&port->sem);
	return retval;
}

static int serial_write_room (struct tty_struct *tty) 
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	int retval = -EINVAL;

	if (!serial)
		return -ENODEV;

	down (&port->sem);

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (serial->type->write_room)
		retval = serial->type->write_room(port);
	else
		retval = generic_write_room(port);

exit:
	up (&port->sem);
	return retval;
}

static int serial_chars_in_buffer (struct tty_struct *tty) 
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	int retval = -EINVAL;

	if (!serial)
		return -ENODEV;

	down (&port->sem);

	dbg("%s = port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (serial->type->chars_in_buffer)
		retval = serial->type->chars_in_buffer(port);
	else
		retval = generic_chars_in_buffer(port);

exit:
	up (&port->sem);
	return retval;
}

static void serial_throttle (struct tty_struct * tty)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial)
		return;

	down (&port->sem);

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg ("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function */
	if (serial->type->throttle)
		serial->type->throttle(port);

exit:
	up (&port->sem);
}

static void serial_unthrottle (struct tty_struct * tty)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial)
		return;

	down (&port->sem);

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function */
	if (serial->type->unthrottle)
		serial->type->unthrottle(port);

exit:
	up (&port->sem);
}

static int serial_ioctl (struct tty_struct *tty, struct file * file, unsigned int cmd, unsigned long arg)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
	int retval = -ENODEV;

	if (!serial)
		return -ENODEV;

	down (&port->sem);

	dbg("%s - port %d, cmd 0x%.4x", __FUNCTION__, port->number, cmd);

	if (!port->open_count) {
		dbg ("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (serial->type->ioctl)
		retval = serial->type->ioctl(port, file, cmd, arg);
	else
		retval = -ENOIOCTLCMD;

exit:
	up (&port->sem);
	return retval;
}

static void serial_set_termios (struct tty_struct *tty, struct termios * old)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial)
		return;

	down (&port->sem);

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (serial->type->set_termios)
		serial->type->set_termios(port, old);

exit:
	up (&port->sem);
}

static void serial_break (struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = (struct usb_serial_port *) tty->driver_data;
	struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

	if (!serial)
		return;

	down (&port->sem);

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (!port->open_count) {
		dbg("%s - port not open", __FUNCTION__);
		goto exit;
	}

	/* pass on to the driver specific version of this function if it is available */
	if (serial->type->break_ctl)
		serial->type->break_ctl(port, break_state);

exit:
	up (&port->sem);
}
#if 0
static void serial_shutdown (struct usb_serial *serial)
{
	dbg ("%s", __FUNCTION__);

	if (serial->type->shutdown)
		serial->type->shutdown(serial);
	else
		generic_shutdown(serial);
}
#endif
static int serial_read_proc (char *page, char **start, off_t off, int count, int *eof, void *data)
{
	struct usb_serial *serial;
	int length = 0;
	int i;
	off_t begin = 0;
	char tmp[40];

	dbg("%s", __FUNCTION__);
	length += sprintf (page, "usbserinfo:1.0 driver:%s\n", DRIVER_VERSION);
	for (i = 0; i < SERIAL_TTY_MINORS && length < PAGE_SIZE; ++i) {
		serial = get_serial_by_minor(i);
		if (serial == NULL)
			continue;

		length += sprintf (page+length, "%d:", i);
		if (serial->type->owner)
			length += sprintf (page+length, " module:%s", serial->type->owner->name);
		length += sprintf (page+length, " name:\"%s\"", serial->type->name);
		length += sprintf (page+length, " vendor:%04x product:%04x", serial->vendor, serial->product);
		length += sprintf (page+length, " num_ports:%d", serial->num_ports);
		length += sprintf (page+length, " port:%d", i - serial->minor + 1);

		usb_make_path(serial->dev, tmp, sizeof(tmp));
		length += sprintf (page+length, " path:%s", tmp);
			
		length += sprintf (page+length, "\n");
		if ((length + begin) > (off + count))
			goto done;
		if ((length + begin) < off) {
			begin += length;
			length = 0;
		}
	}
	*eof = 1;
done:
	if (off >= (length + begin))
		return 0;
	*start = page + (off-begin);
	return ((count < begin+length-off) ? count : begin+length-off);
}


/*-----------------------------------------------------------*/
static int generic_write (struct usb_serial_port *port, int from_user, const unsigned char *buf, int count)
{
        struct usb_serial *serial = port->serial;
        int result;

        dbg("%s - port %d", __FUNCTION__, port->number);

        if (count == 0) {
                dbg("%s - write request of 0 bytes", __FUNCTION__);
                return (0);
        }

        /* only do something if we have a bulk out endpoint */
        if (serial->num_bulk_out) {
                if (port->write_urb->status == -EINPROGRESS) {
                        dbg("%s - already writing", __FUNCTION__);
                        return (0);
                }

                count = (count > port->bulk_out_size) ? port->bulk_out_size : count;

                if (from_user) {
                        if (copy_from_user(port->write_urb->transfer_buffer, buf, count))
                                return -EFAULT;
                }
                else {
                        memcpy (port->write_urb->transfer_buffer, buf, count);
                }

                usb_serial_debug_data (__FILE__, __FUNCTION__, count, port->write_urb->transfer_buffer);

                /* set up our urb */
                usb_fill_bulk_urb (port->write_urb, serial->dev,
                                   usb_sndbulkpipe (serial->dev,
                                                    port->bulk_out_endpointAddress),
                                   port->write_urb->transfer_buffer, count,
                                   ((serial->type->write_bulk_callback) ?
                                     serial->type->write_bulk_callback :
                                     generic_write_bulk_callback), port);

                /* send the data out the bulk port */
                result = usb_submit_urb(port->write_urb);
                if (result)
                        err("%s - failed submitting write urb, error %d", __FUNCTION__, result);
                else
                        result = count;

                return result;
        }

        /* no bulk out, so return 0 bytes written */
        return (0);
}

static int generic_write_room (struct usb_serial_port *port)
{
        struct usb_serial *serial = port->serial;
        int room = 0;

        dbg("%s - port %d", __FUNCTION__, port->number);

        if (serial->num_bulk_out) {
                if (port->write_urb->status != -EINPROGRESS)
                        room = port->bulk_out_size;
        }

        dbg("%s - returns %d", __FUNCTION__, room);
        return (room);
}

static int generic_chars_in_buffer (struct usb_serial_port *port)
{
        struct usb_serial *serial = port->serial;
        int chars = 0;

        dbg("%s - port %d", __FUNCTION__, port->number);

        if (serial->num_bulk_out) {
                if (port->write_urb->status == -EINPROGRESS)
                        chars = port->write_urb->transfer_buffer_length;
        }

        dbg("%s - returns %d", __FUNCTION__, chars);
        return (chars);
}
#if 0
static void generic_shutdown (struct usb_serial *serial)
{
        int i;

        dbg("%s", __FUNCTION__);

        /* stop reads and writes on all ports */
        for (i=0; i < serial->num_ports; ++i) {
                generic_cleanup (&serial->port[i]);
        }
}
static void generic_cleanup (struct usb_serial_port *port)
{
        struct usb_serial *serial = port->serial;

        dbg("%s - port %d", __FUNCTION__, port->number);

        if (serial->dev) {
                /* shutdown any bulk reads that might be going on */
                if (serial->num_bulk_out)
                        usb_unlink_urb (port->write_urb);
                if (serial->num_bulk_in)
                        usb_unlink_urb (port->read_urb);
        }
}
#endif
/*----------------------------------------------------------*/
static void generic_write_bulk_callback (struct urb *urb)
{
        struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
        struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);

        dbg("%s - port %d", __FUNCTION__, port->number);

        if (!serial) {
                dbg("%s - bad serial pointer, exiting", __FUNCTION__);
                return;
        }

        if (urb->status) {
                dbg("%s - nonzero write bulk status received: %d", __FUNCTION__, urb->status);
                return;
        }

        queue_task(&port->tqueue, &tq_immediate);
        mark_bh(IMMEDIATE_BH);

        return;
}

static struct usb_serial *get_free_serial (int num_ports, int *minor)
{
        struct usb_serial *serial = NULL;
        int i, j;
        int good_spot;

        dbg("%s %d", __FUNCTION__, num_ports);

        *minor = 0;
        for (i = 0; i < SERIAL_TTY_MINORS; ++i) {
                if (serial_table[i])
                        continue;

                good_spot = 1;
                for (j = 1; j <= num_ports-1; ++j)
                        if (serial_table[i+j])
                                good_spot = 0;
                if (good_spot == 0)
                        continue;

                if (!(serial = kmalloc(sizeof(struct usb_serial), GFP_KERNEL))) {
                        err("%s - Out of memory", __FUNCTION__);
                        return NULL;
                }
                memset(serial, 0, sizeof(struct usb_serial));
                serial->magic = USB_SERIAL_MAGIC;
                serial_table[i] = serial;
                *minor = i;
                dbg("%s - minor base = %d", __FUNCTION__, *minor);
                for (i = *minor+1; (i < (*minor + num_ports)) && (i < SERIAL_TTY_MINORS); ++i)
                        serial_table[i] = serial;
                return serial;
        }
        return NULL;
}

static void return_serial (struct usb_serial *serial)
{
        int i;

        dbg("%s", __FUNCTION__);

        if (serial == NULL)
                return;

        for (i = 0; i < serial->num_ports; ++i) {
                serial_table[serial->minor + i] = NULL;
        }

        return;
}

static void port_softint(void *private)
{
        struct usb_serial_port *port = (struct usb_serial_port *)private;
        struct usb_serial *serial = get_usb_serial (port, __FUNCTION__);
        struct tty_struct *tty;

        dbg("%s - port %d", __FUNCTION__, port->number);

        if (!serial)
                return;

        tty = port->tty;
        if (!tty)
                return;

        if ((tty->flags & (1 << TTY_DO_WRITE_WAKEUP)) && tty->ldisc.write_wakeup) {
                dbg("%s - write wakeup call.", __FUNCTION__);
                (tty->ldisc.write_wakeup)(tty);
        }

        wake_up_interruptible(&tty->write_wait);
}


static void * usb_serial_probe(struct usb_device *dev, unsigned int ifnum,
                               const struct usb_device_id *id)
{
        struct usb_serial *serial = NULL;
        struct usb_serial_port *port;
        struct usb_interface *interface;
        struct usb_interface_descriptor *iface_desc;
        struct usb_endpoint_descriptor *endpoint;
        struct usb_endpoint_descriptor *interrupt_in_endpoint[MAX_NUM_PORTS];
        struct usb_endpoint_descriptor *bulk_in_endpoint[MAX_NUM_PORTS];
        struct usb_endpoint_descriptor *bulk_out_endpoint[MAX_NUM_PORTS];
        struct usb_serial_device_type *type = NULL;
        struct list_head *tmp;
        int found;
        int minor;
        int buffer_size;
        int i;
        int num_interrupt_in = 0;
        int num_bulk_in = 0;
        int num_bulk_out = 0;
        int num_ports;
        int max_endpoints;
        const struct usb_device_id *id_pattern = NULL;

        /* loop through our list of known serial converters, and see if this
           device matches. */
        found = 0;
        interface = &dev->actconfig->interface[ifnum];
        list_for_each (tmp, &usb_serial_driver_list) {
                type = list_entry(tmp, struct usb_serial_device_type, driver_list);
                id_pattern = usb_match_id(dev, interface, type->id_table);
                if (id_pattern != NULL) {
                        dbg("descriptor matches");
                        found = 1;
                        break;
                }
        }
        if (!found) {
                /* no match */
                dbg("none matched");
                return(NULL);
        }
        /* descriptor matches, let's find the endpoints needed */
        /* check out the endpoints */
        iface_desc = &interface->altsetting[0];
        for (i = 0; i < iface_desc->bNumEndpoints; ++i) {
                endpoint = &iface_desc->endpoint[i];

                if ((endpoint->bEndpointAddress & 0x80) &&
                    ((endpoint->bmAttributes & 3) == 0x02)) {
                        /* we found a bulk in endpoint */
                        dbg("found bulk in");
                        bulk_in_endpoint[num_bulk_in] = endpoint;
                        ++num_bulk_in;
                }

                if (((endpoint->bEndpointAddress & 0x80) == 0x00) &&
                    ((endpoint->bmAttributes & 3) == 0x02)) {
                        /* we found a bulk out endpoint */
                        dbg("found bulk out");
                        bulk_out_endpoint[num_bulk_out] = endpoint;
                        ++num_bulk_out;
                }

                if ((endpoint->bEndpointAddress & 0x80) &&
                    ((endpoint->bmAttributes & 3) == 0x03)) {
                        /* we found a interrupt in endpoint */
                        dbg("found interrupt in");
                        interrupt_in_endpoint[num_interrupt_in] = endpoint;
                        ++num_interrupt_in;
                }
        }

#if defined(CONFIG_USB_SERIAL_PL2303) || defined(CONFIG_USB_SERIAL_PL2303_MODULE)
        /* BEGIN HORRIBLE HACK FOR PL2303 */
        /* this is needed due to the looney way its endpoints are set up */
        if (((dev->descriptor.idVendor == PL2303_VENDOR_ID) &&
             (dev->descriptor.idProduct == PL2303_PRODUCT_ID)) ||
            ((dev->descriptor.idVendor == ATEN_VENDOR_ID) &&
             (dev->descriptor.idProduct == ATEN_PRODUCT_ID))) {
                if (ifnum == 1) {
                        /* check out the endpoints of the other interface*/
                        interface = &dev->actconfig->interface[ifnum ^ 1];
                        iface_desc = &interface->altsetting[0];
                        for (i = 0; i < iface_desc->bNumEndpoints; ++i) {
                                endpoint = &iface_desc->endpoint[i];
                                if ((endpoint->bEndpointAddress & 0x80) &&
                                    ((endpoint->bmAttributes & 3) == 0x03)) {
                                        /* we found a interrupt in endpoint */
                                        dbg("found interrupt in for Prolific device on separate interface");
                                        interrupt_in_endpoint[num_interrupt_in] = endpoint;
                                        ++num_interrupt_in;
                                }
                        }
                }

                /* Now make sure the PL-2303 is configured correctly.
                 * If not, give up now and hope this hack will work
                 * properly during a later invocation of usb_serial_probe
                 */
                if (num_bulk_in == 0 || num_bulk_out == 0) {
                        info("PL-2303 hack: descriptors matched but endpoints did not");
                        return NULL;
                }
        }
        /* END HORRIBLE HACK FOR PL2303 */
#endif

        /* found all that we need */
        info("%s converter detected", type->name);

#ifdef CONFIG_USB_SERIAL_SIERRAWIRELESS
        if (type == &sierra_driver) {
                num_ports = num_bulk_out;
                if (num_ports == 0) {
                        err("Sierra 3G device with no bulk out, not allowed.");
                        return NULL;
                }
        } else
#endif
                num_ports = type->num_ports;

        serial = get_free_serial (num_ports, &minor);
        if (serial == NULL) {
                err("No more free serial devices");
                return NULL;
        }

        serial->dev = dev;
        serial->type = type;
        serial->interface = interface;
        serial->minor = minor;
        serial->num_ports = num_ports;
        serial->num_bulk_in = num_bulk_in;
        serial->num_bulk_out = num_bulk_out;
        serial->num_interrupt_in = num_interrupt_in;
        serial->vendor = dev->descriptor.idVendor;
        serial->product = dev->descriptor.idProduct;

        /* set up the endpoint information */
        for (i = 0; i < num_bulk_in; ++i) {
                endpoint = bulk_in_endpoint[i];
                port = &serial->port[i];
                port->read_urb = usb_alloc_urb (0);
                if (!port->read_urb) {
                        err("No free urbs available");
                        goto probe_error;
                }
//Amin marked           buffer_size = endpoint->wMaxPacketSize;
// ===> 20060310 Amin modify for improve EVDO and HSDPA Card
                buffer_size = 2048;
                printk("KERNEL DEBUG => USBSERIAL.O buffer_size = 2048\n");
// <=== 20060310 Amin modify for improve EVDO and HSDPA Card
                port->bulk_in_endpointAddress = endpoint->bEndpointAddress;
                port->bulk_in_buffer = kmalloc (buffer_size, GFP_KERNEL);
                if (!port->bulk_in_buffer) {
                        err("Couldn't allocate bulk_in_buffer");
                        goto probe_error;
                }
                usb_fill_bulk_urb (port->read_urb, dev,
                                   usb_rcvbulkpipe (dev,
                                                    endpoint->bEndpointAddress),
                                   port->bulk_in_buffer, buffer_size,
                                   ((serial->type->read_bulk_callback) ?
                                     serial->type->read_bulk_callback :
                                     sw_usb_serial_generic_read_bulk_callback),
                                   port);
        }

        for (i = 0; i < num_bulk_out; ++i) {
                endpoint = bulk_out_endpoint[i];
                port = &serial->port[i];
                port->write_urb = usb_alloc_urb(0);
                if (!port->write_urb) {
                        err("No free urbs available");
                        goto probe_error;
                }
                buffer_size = endpoint->wMaxPacketSize;
                port->bulk_out_size = buffer_size;
                port->bulk_out_endpointAddress = endpoint->bEndpointAddress;
                port->bulk_out_buffer = kmalloc (buffer_size, GFP_KERNEL);
                if (!port->bulk_out_buffer) {
                        err("Couldn't allocate bulk_out_buffer");
                        goto probe_error;
                }
                usb_fill_bulk_urb (port->write_urb, dev,
                                   usb_sndbulkpipe (dev,
                                                    endpoint->bEndpointAddress),
                                   port->bulk_out_buffer, buffer_size,
                                   ((serial->type->write_bulk_callback) ?
                                     serial->type->write_bulk_callback :
                                     generic_write_bulk_callback),
                                   port);
        }

        for (i = 0; i < num_interrupt_in; ++i) {
                endpoint = interrupt_in_endpoint[i];
                port = &serial->port[i];
                port->interrupt_in_urb = usb_alloc_urb(0);
                if (!port->interrupt_in_urb) {
                        err("No free urbs available");
                        goto probe_error;
                }
                buffer_size = endpoint->wMaxPacketSize;
                port->interrupt_in_endpointAddress = endpoint->bEndpointAddress;
                port->interrupt_in_buffer = kmalloc (buffer_size, GFP_KERNEL);
                if (!port->interrupt_in_buffer) {
                        err("Couldn't allocate interrupt_in_buffer");
                        goto probe_error;
                }
                usb_fill_int_urb (port->interrupt_in_urb, dev,
                                  usb_rcvintpipe (dev,
                                                  endpoint->bEndpointAddress),
                                  port->interrupt_in_buffer, buffer_size,
                                  serial->type->read_int_callback, port,
                                  endpoint->bInterval);
        }

        /* initialize some parts of the port structures */
        /* we don't use num_ports here cauz some devices have more endpoint pairs than ports */
        max_endpoints = max(num_bulk_in, num_bulk_out);
        max_endpoints = max(max_endpoints, num_interrupt_in);
        max_endpoints = max(max_endpoints, (int)serial->num_ports);
        dbg("%s - setting up %d port structures for this device", __FUNCTION__, max_endpoints);
        for (i = 0; i < max_endpoints; ++i) {
                port = &serial->port[i];
                port->number = i + serial->minor;
                port->serial = serial;
                port->magic = USB_SERIAL_PORT_MAGIC;
                port->tqueue.routine = port_softint;
                port->tqueue.data = port;
                init_MUTEX (&port->sem);
        }

        /* if this device type has a startup function, call it */
        if (type->startup) {
                i = type->startup (serial);
                if (i < 0)
                        goto probe_error;
                if (i > 0)
                        return serial;
        }

        /* initialize the devfs nodes for this device and let the user know what ports we are bound to */
        for (i = 0; i < serial->num_ports; ++i) {
                tty_register_devfs (&serial_tty_driver, 0, serial->port[i].number);
                info("%s converter now attached to ttyUSB%d (or usb/tts/%d for devfs)",
                     type->name, serial->port[i].number, serial->port[i].number);
        }

        return serial; /* success */


probe_error:
        for (i = 0; i < num_bulk_in; ++i) {
                port = &serial->port[i];
                if (port->read_urb)
                        usb_free_urb (port->read_urb);
                if (port->bulk_in_buffer)
                        kfree (port->bulk_in_buffer);
        }
        for (i = 0; i < num_bulk_out; ++i) {
                port = &serial->port[i];
                if (port->write_urb)
                        usb_free_urb (port->write_urb);
                if (port->bulk_out_buffer)
                        kfree (port->bulk_out_buffer);
        }
        for (i = 0; i < num_interrupt_in; ++i) {
                port = &serial->port[i];
                if (port->interrupt_in_urb)
                        usb_free_urb (port->interrupt_in_urb);
                if (port->interrupt_in_buffer)
                        kfree (port->interrupt_in_buffer);
        }

        /* return the minor range that this device had */
        return_serial (serial);

        /* free up any memory that we allocated */
        kfree (serial);
        return NULL;
}

static void usb_serial_disconnect(struct usb_device *dev, void *ptr)
{
        struct usb_serial *serial = (struct usb_serial *) ptr;
        struct usb_serial_port *port;
        int i;

        dbg ("%s", __FUNCTION__);
        if (serial) {
                /* fail all future close/read/write/ioctl/etc calls */
                for (i = 0; i < serial->num_ports; ++i) {
                        port = &serial->port[i];
                        down (&port->sem);
                        if (port->tty != NULL) {
                                while (port->open_count > 0) {
                                        //__serial_close(port, NULL);
					sw_usb_serial_generic_close(port,NULL);
                                }
                                port->tty->driver_data = NULL;
                        }
                        up (&port->sem);
                }

                serial->dev = NULL;
                //serial_shutdown (serial);
		sw_usb_serial_generic_shutdown(serial);

                for (i = 0; i < serial->num_ports; ++i)
                        serial->port[i].open_count = 0;

                for (i = 0; i < serial->num_bulk_in; ++i) {
                        port = &serial->port[i];
                        if (port->read_urb) {
                                usb_unlink_urb (port->read_urb);
                                usb_free_urb (port->read_urb);
                        }
                        if (port->bulk_in_buffer)
                                kfree (port->bulk_in_buffer);
                }
                for (i = 0; i < serial->num_bulk_out; ++i) {
                        port = &serial->port[i];
                        if (port->write_urb) {
                                usb_unlink_urb (port->write_urb);
                                usb_free_urb (port->write_urb);
                        }
                        if (port->bulk_out_buffer)
                                kfree (port->bulk_out_buffer);
                }
                for (i = 0; i < serial->num_interrupt_in; ++i) {
                        port = &serial->port[i];
                        if (port->interrupt_in_urb) {
                                usb_unlink_urb (port->interrupt_in_urb);
                                usb_free_urb (port->interrupt_in_urb);
                        }
                        if (port->interrupt_in_buffer)
                                kfree (port->interrupt_in_buffer);
                }

                for (i = 0; i < serial->num_ports; ++i) {
                        tty_unregister_devfs (&serial_tty_driver, serial->port[i].number);
                        info("%s converter now disconnected from ttyUSB%d", serial->type->name, serial->port[i].number);
                }

                /* return the minor range that this device had */
                return_serial (serial);

                /* free up any memory that we allocated */
                kfree (serial);

        } else {
                info("device disconnected");
        }

}

#if 0
static int sw_attach(struct usb_serial *serial)
{
	struct usb_device *hdev = serial->dev;
	int rc;

	dbg("%s - serial(0x%p)", __FUNCTION__, serial);
	
	rc = usb_control_msg(
		hdev, 
		usb_sndctrlpipe(hdev, 0),
		USB_VENDER_REQUEST_SET_DEVICE_POWER_STATE,	/* bRequest */ 
		USB_TYPE_VENDOR|USB_RECIP_DEVICE, 		/* bmRequestType */
		USB_DEVICE_POWER_STATE_D0, 			/* wValue */
		0,						/* wIndex */
		NULL, 						/* Data */
		0, 						/* wLength */
		1000);						/* Timeout */

	err("%s - rc(%d)", __FUNCTION__, rc);
	return rc;
}
#endif
//void sw_usb_serial_generic_read_bulk_callback (struct urb *urb, struct pt_regs *regs)
static void sw_usb_serial_generic_read_bulk_callback (struct urb *urb)
{
	struct usb_serial_port *port = (struct usb_serial_port *)urb->context;
	struct usb_serial *serial = port->serial;
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	int result;
	int i;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (urb->status) {
		dbg("%s - nonzero read bulk status received: %d", __FUNCTION__, urb->status);
		return;
	}

	//usb_serial_dbg_data(__FILE__,  __FUNCTION__, urb->actual_length, data);

	tty = port->tty;
	if (tty && urb->actual_length) {
	   #if 0
		tty_buffer_request_room(tty, urb->actual_length);
		tty_insert_flip_string(tty, data, urb->actual_length);
	  	tty_flip_buffer_push(tty);
           #endif
           #if 1
                for (i = 0; i < urb->actual_length ; ++i) {
                        /* if we insert more than TTY_FLIPBUF_SIZE characters, we drop them. */
                        if(tty->flip.count >= TTY_FLIPBUF_SIZE) {
                                tty_flip_buffer_push(tty);
                        }
                        /* this doesn't actually push the data through unless tty->low_latency is set */
                        tty_insert_flip_char(tty, data[i], 0);
                }
                tty_flip_buffer_push(tty);
           #endif

	}
	else
		dbg("%s: empty read urb received", __FUNCTION__);

	/* Continue trying to always read  */
	usb_fill_bulk_urb (port->read_urb, serial->dev,
			   usb_rcvbulkpipe (serial->dev,
				   	    port->bulk_in_endpointAddress),
			   port->read_urb->transfer_buffer,
			   port->read_urb->transfer_buffer_length,
			   ((serial->type->read_bulk_callback) ? 
			     serial->type->read_bulk_callback : 
			     sw_usb_serial_generic_read_bulk_callback), port);
	result = usb_submit_urb(port->read_urb);
        //result = usb_submit_urb(port->read_urb, GFP_ATOMIC); //for kernel 2.6
	if (result)
		dbg("%s - failed resubmitting read urb, error %d\n", __FUNCTION__, result);
}
#if 1
static int generic_open (struct usb_serial_port *port, struct file *filp)
{
	struct usb_serial *serial = port->serial;
	int result = 0;

	dbg("%s - port %d", __FUNCTION__, port->number);

	/* force low_latency on so that our tty_push actually forces the data through, 
	   otherwise it is scheduled, and with high data rates (like with OHCI) data
	   can get lost. */
	if (port->tty)
		port->tty->low_latency = 1;

	/* if we have a bulk interrupt, start reading from it */
	if (serial->num_bulk_in) {
		/* Start reading from the device */
		usb_fill_bulk_urb (port->read_urb, serial->dev,
				   usb_rcvbulkpipe(serial->dev, port->bulk_in_endpointAddress),
				   port->read_urb->transfer_buffer,
				   port->read_urb->transfer_buffer_length,
				   ((serial->type->read_bulk_callback) ?
				     serial->type->read_bulk_callback :
				     sw_usb_serial_generic_read_bulk_callback),
				   port);
		result = usb_submit_urb(port->read_urb); //, GFP_KERNEL);
		if (result)
			//dev_err(&port->dev, "%s - failed resubmitting read urb, error %d\n", __FUNCTION__, result);
			dbg("%s - failed resubmitting read urb, error %d\n", __FUNCTION__, result);

	}

	return result;
}
#endif
int sw_usb_serial_generic_open (struct usb_serial_port *port, struct file *filp)
{
	int rc;
	struct usb_serial *serial = port->serial;
	struct usb_device *hdev = serial->dev;

	dbg("%s - port %d", __FUNCTION__, port->number);

	rc = generic_open(port, filp);
	err("%s - rc(%d)", __FUNCTION__, rc);

	if(0 == rc)
	{
		rc = usb_control_msg(
			hdev, 
			usb_sndctrlpipe(hdev, 0),
			SET_CONTROL_LINE_STATE,				/* bRequest */ 
			USB_TYPE_CLASS|USB_RECIP_INTERFACE, 		/* bmRequestType */
			ACM_CTRL_DTR|ACM_CTRL_RTS,			/* wValue */
			0,						/* wIndex */
			NULL, 						/* Data */
			0, 						/* wLength */
			1000);						/* Timeout */
		err("%s - usb_control_msg: rc(%d)", __FUNCTION__, rc);
	}

	return rc;
}

static void generic_cleanup (struct usb_serial_port *port)
{
	struct usb_serial *serial = port->serial;

	dbg("%s - port %d", __FUNCTION__, port->number);

	if (serial->dev) {
		/* shutdown any bulk reads that might be going on */
		if (serial->num_bulk_out)
			usb_unlink_urb(port->write_urb);
			//usb_kill_urb(port->write_urb);
		if (serial->num_bulk_in)
			usb_unlink_urb(port->read_urb);
			//usb_kill_urb(port->read_urb);
	}
}

static void sw_usb_serial_generic_close (struct usb_serial_port *port, struct file * filp)
{
	int rc;
	struct usb_serial *serial = port->serial;
	struct usb_device *hdev = serial->dev;

	dbg("%s - port %d", __FUNCTION__, port->number);

	rc = usb_control_msg(
		hdev, 
		usb_sndctrlpipe(hdev, 0),
		SET_CONTROL_LINE_STATE,				/* bRequest */ 
		USB_TYPE_CLASS|USB_RECIP_INTERFACE, 		/* bmRequestType */
		0, 						/* wValue */
		0,						/* wIndex */
		NULL, 						/* Data */
		0, 						/* wLength */
		1000);						/* Timeout */
	err("%s - rc(%d)", __FUNCTION__, rc);
	
	generic_cleanup (port);
}

static void sw_usb_serial_generic_shutdown(struct usb_serial *serial)
{
	int i, rc;
	struct usb_device *hdev = serial->dev;

	dbg("%s serial(0x%p)", __FUNCTION__, serial);

	if(hdev)
	{
		rc = usb_control_msg(
			hdev, 
			usb_sndctrlpipe(hdev, 0),
			USB_VENDER_REQUEST_SET_DEVICE_POWER_STATE,	/* bRequest */ 
			USB_TYPE_VENDOR|USB_RECIP_DEVICE, 		/* bmRequestType */
			USB_DEVICE_POWER_STATE_D3, 			/* wValue */
			0,						/* wIndex */
			NULL, 						/* Data */
			0, 						/* wLength */
			1000);						/* Timeout */
		err("%s - rc(%d)", __FUNCTION__, rc);
	}

	/* stop reads and writes on all ports */
	for (i=0; i < serial->num_ports; ++i) {
		generic_cleanup(&serial->port[i]);
	}
}
int usb_serial_register(struct usb_serial_device_type *new_device)
{
        /* Add this device to our list of devices */
        list_add(&new_device->driver_list, &usb_serial_driver_list);

        info ("USB Serial support registered for %s", new_device->name);

        usb_scan_devices();

        return 0;
}


void usb_serial_deregister(struct usb_serial_device_type *device)
{
        struct usb_serial *serial;
        int i;

        info("USB Serial deregistering driver %s", device->name);

        /* clear out the serial_table if the device is attached to a port */
        for(i = 0; i < SERIAL_TTY_MINORS; ++i) {
                serial = serial_table[i];
                if ((serial != NULL) && (serial->type == device)) {
                        usb_driver_release_interface (&sierra_driver, serial->interface);
                        usb_serial_disconnect (NULL, serial);
                }
        }

        list_del(&device->driver_list);
}

static int __init sierra_init(void)
{
	int retval;
	int i;

        /* Initalize our global data */
        for (i = 0; i < SERIAL_TTY_MINORS; ++i) {
                serial_table[i] = NULL;
        }

        /* register the tty driver */
        serial_tty_driver.init_termios          = tty_std_termios;
        serial_tty_driver.init_termios.c_cflag  = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
        if (tty_register_driver (&serial_tty_driver)) {
                err("%s - failed to register tty driver", __FUNCTION__);
                return -1;
        }

	retval = usb_serial_register(&sierra_device);
	if (retval)
	{
		tty_unregister_driver(&serial_tty_driver);
		printk("%s return usb_serial_register. retval=[%d].\n",__FUNCTION__, retval);
		return retval;
	}
	retval = usb_register(&sierra_driver);
        if (retval){
		usb_serial_deregister(&sierra_device);
                tty_unregister_driver(&serial_tty_driver);
                err("usb_register failed for the Sierra 3G USB-Serial driver. Error number %d\n", retval);
                return -1;
        }

	return retval;
}

static void __exit sierra_exit(void)
{
	usb_deregister(&sierra_driver);
	usb_serial_deregister(&sierra_device);
}

module_init(sierra_init);
module_exit(sierra_exit);
MODULE_LICENSE("GPL");
