/*
** mrb_bsdiic.c - BsdIic class
**
** Copyright (c) Hiroki Mori 2016
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/data.h"
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
  mrb_int addr, reg;
  struct iiccmd cmd;
  int error;
  char cmdbuf = 0;

  mrb_get_args(mrb, "ii", &addr, &reg);

  bzero(&cmd, sizeof(cmd));
  cmd.slave = addr << 1;
  cmd.count = 1;
  cmd.last = 0;
  cmd.buf = &cmdbuf;
  cmdbuf = reg;
  error = ioctl(data->fd, I2CSTART, &cmd);
  error = ioctl(data->fd, I2CWRITE, &cmd);
  error = ioctl(data->fd, I2CSTOP);

  cmd.slave = addr << 1;
  error = ioctl(data->fd, I2CSTART, &cmd);
  error = ioctl(data->fd, I2CSTOP);
  error = read(data->fd, &cmdbuf, 1);

  return mrb_fixnum_value(cmdbuf);
}

static mrb_value mrb_bsdiic_write(mrb_state *mrb, mrb_value self)
{
  mrb_bsdiic_data *data = DATA_PTR(self);
  mrb_int addr, reg, val;
  struct iiccmd cmd;
  int error;
  char cmdbuf[2];

  mrb_get_args(mrb, "iii", &addr, &reg, &val);

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

  return mrb_fixnum_value(0);
}

void mrb_mruby_bsdiic_gem_init(mrb_state *mrb)
{
    struct RClass *bsdiic;
    bsdiic = mrb_define_class(mrb, "BsdIic", mrb->object_class);
    mrb_define_method(mrb, bsdiic, "initialize", mrb_bsdiic_init, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, bsdiic, "read", mrb_bsdiic_read, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, bsdiic, "write", mrb_bsdiic_write, MRB_ARGS_REQ(3));
    DONE;
}

void mrb_mruby_bsdiic_gem_final(mrb_state *mrb)
{
}

