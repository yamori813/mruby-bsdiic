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

#include <fcntl.h>
#include <unistd.h>

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
  char cmdbuf;

  mrb_get_args(mrb, "ii", &addr, &reg);

  cmd.slave = addr;
  cmd.count = 1;
  cmd.last = 0;
  cmd.buf = &cmdbuf;
  error = ioctl(data->fd, I2CSTART, &cmd);
  error = ioctl(data->fd, I2CREAD, &cmd);
  error = ioctl(data->fd, I2CSTOP);
//  return mrb_fixnum_value(cmdbuf);
  return mrb_fixnum_value(data->fd);
}

void mrb_mruby_bsdiic_gem_init(mrb_state *mrb)
{
    struct RClass *bsdiic;
    bsdiic = mrb_define_class(mrb, "BsdIic", mrb->object_class);
    mrb_define_method(mrb, bsdiic, "initialize", mrb_bsdiic_init, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, bsdiic, "read", mrb_bsdiic_read, MRB_ARGS_REQ(2));
    DONE;
}

void mrb_mruby_bsdiic_gem_final(mrb_state *mrb)
{
}

