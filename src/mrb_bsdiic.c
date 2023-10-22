/*
** mrb_bsdiic.c - BsdIic class
**
** Copyright (c) Hiroki Mori 2016
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mrb_bsdiic.h"

#include <err.h>
#include <errno.h>
#include <sysexits.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <dev/iicbus/iic.h>

#define DONE mrb_gc_arena_restore(mrb, 0);

typedef struct {
  int fd;
} mrb_bsdiic_data;

static const struct mrb_data_type mrb_bsdiic_data_type = {
  "mrb_bsdiic_data", mrb_free,
};

static mrb_value mrb_bsdiic_init(mrb_state *mrb, mrb_value self)
{
  mrb_bsdiic_data *data;
  char device[16];
  mrb_int num;

  data = (mrb_bsdiic_data *)DATA_PTR(self);
  if (data) {
    mrb_free(mrb, data);
  }
  DATA_TYPE(self) = &mrb_bsdiic_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "i", &num);
  data = (mrb_bsdiic_data *)mrb_malloc(mrb, sizeof(mrb_bsdiic_data));
  snprintf(device, sizeof(device), "/dev/iic%u", num);
  data->fd = open(device, O_RDWR); 
  DATA_PTR(self) = data;

  return self;
}

static mrb_value mrb_bsdiic_read(mrb_state *mrb, mrb_value self)
{
  mrb_bsdiic_data *data = DATA_PTR(self);
  mrb_int addr, len;
  mrb_value arr, res;
#ifdef USE_RDWR
  struct iic_msg msg[2];
  struct iic_rdwr_data rdwr;
#else
  struct iiccmd cmd;
#endif
  int error;
  char cmdbuf[MAX_WRITES_SIZE];
  unsigned char rdbuf[MAX_READ_SIZE];
  int i, size;

  mrb_get_args(mrb, "ii|A", &addr, &len, &arr);

#ifdef USE_RDWR
  if (mrb_get_argc(mrb) == 3) {
    size = RARRAY_LEN( arr );
    msg[0].len = size;
    msg[0].buf = cmdbuf;
    for (i = 0; i < size; ++i)
      cmdbuf[i] = mrb_fixnum( mrb_ary_ref( mrb, arr, i ) );
    msg[0].slave = addr << 1;
    msg[0].flags = IIC_M_WR;
    msg[1].slave = addr << 1;
    msg[1].flags = IIC_M_RD;
    msg[1].len = len;
    msg[1].buf = rdbuf;
    rdwr.msgs = msg;
    rdwr.nmsgs = 2;
  } else {
    msg[0].slave = addr << 1;
    msg[0].flags = IIC_M_RD;
    msg[0].len = len;
    msg[0].buf = rdbuf;
    rdwr.msgs = msg;
    rdwr.nmsgs = 1;
  }
  error = ioctl(data->fd, I2CRDWR, &rdwr);
#else
  if (mrb_get_argc(mrb) == 3) {
    bzero(cmdbuf, sizeof(cmdbuf));
    size = RARRAY_LEN( arr );
    for (i = 0; i < size; ++i)
      cmdbuf[i] = mrb_fixnum( mrb_ary_ref( mrb, arr, i ) );
    cmd.slave = addr << 1;
    cmd.count = size;
    cmd.last = 0;
    cmd.buf = cmdbuf;
    error = ioctl(data->fd, I2CSTART, &cmd);
    error = ioctl(data->fd, I2CWRITE, &cmd);
    error = ioctl(data->fd, I2CSTOP);
  }

  cmd.slave = addr << 1;
  cmd.count = len;
  error = ioctl(data->fd, I2CSTART, &cmd);
  error = ioctl(data->fd, I2CSTOP);
  error = read(data->fd, rdbuf, len);
#endif

  if (error) {
    return mrb_nil_value();
  }

  res = mrb_ary_new(mrb);
  for (i = 0; i < len; ++i) {
    mrb_ary_push(mrb, res, mrb_fixnum_value(rdbuf[i]));
  }
  return res;
}

static mrb_value mrb_bsdiic_write(mrb_state *mrb, mrb_value self)
{
  mrb_bsdiic_data *data = DATA_PTR(self);
  mrb_int addr, reg, val;
  mrb_value arr;
  int len;
#ifdef USE_RDWR
  struct iic_msg msg;
  struct iic_rdwr_data rdwr;
#else
  struct iiccmd cmd;
#endif
  int error;
  char cmdbuf[MAX_WRITES_SIZE];
  int i;

  if (mrb_get_argc(mrb) == 2) {
    mrb_get_args(mrb, "iA", &addr, &arr);
    len = RARRAY_LEN( arr );

    if(len < MAX_WRITES_SIZE) {
#ifdef USE_RDWR
      msg.slave = addr << 1;
      msg.flags = IIC_M_WR;
      msg.len = len;
      for (i = 0; i < len; ++i)
        cmdbuf[i] = mrb_fixnum( mrb_ary_ref( mrb, arr, i ) );
      msg.buf = &cmdbuf;
      rdwr.msgs = &msg;
      rdwr.nmsgs = 1;
      error = ioctl(data->fd, I2CRDWR, &rdwr);
#else
      bzero(&cmd, sizeof(cmd));
      cmd.slave = addr << 1;
      cmd.count = len;
      cmd.last = 0;
      cmd.buf = cmdbuf;
      for (i = 0; i < len; ++i)
        cmdbuf[i] = mrb_fixnum( mrb_ary_ref( mrb, arr, i ) );
      error = ioctl(data->fd, I2CSTART, &cmd);
      error = ioctl(data->fd, I2CWRITE, &cmd);
      error = ioctl(data->fd, I2CSTOP);
#endif
    }

    if (error)
      return mrb_fixnum_value(0);
    else
      return mrb_fixnum_value(len);
  } else {

    mrb_get_args(mrb, "iii", &addr, &reg, &val);

#ifdef USE_RDWR
    msg.slave = addr << 1;
    msg.flags = IIC_M_WR;
    msg.len = 2;
    cmdbuf[0] = reg;
    cmdbuf[1] = val;
    msg.buf = &cmdbuf;
    rdwr.msgs = &msg;
    rdwr.nmsgs = 1;
    error = ioctl(data->fd, I2CRDWR, &rdwr);
#else
    bzero(&cmd, sizeof(cmd));
    cmd.slave = addr << 1;
    cmd.count = 2;
    cmd.last = 0;
    cmd.buf = cmdbuf;
    cmdbuf[0] = reg;
    cmdbuf[1] = val;
    error = ioctl(data->fd, I2CSTART, &cmd);
    error = ioctl(data->fd, I2CWRITE, &cmd);
    error = ioctl(data->fd, I2CSTOP);
#endif

    if (error)
      return mrb_fixnum_value(0);
    else
      return mrb_fixnum_value(1);
  }
}

void mrb_mruby_bsdiic_gem_init(mrb_state *mrb)
{
    struct RClass *bsdiic;
    bsdiic = mrb_define_class(mrb, "BsdIic", mrb->object_class);
    mrb_define_method(mrb, bsdiic, "initialize", mrb_bsdiic_init, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, bsdiic, "read", mrb_bsdiic_read, MRB_ARGS_ARG(2, 1));
    mrb_define_method(mrb, bsdiic, "write", mrb_bsdiic_write, MRB_ARGS_ARG(2, 1));
    DONE;
}

void mrb_mruby_bsdiic_gem_final(mrb_state *mrb)
{
}

