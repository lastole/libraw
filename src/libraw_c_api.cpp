/* -*- C++ -*-
 * File: libraw_c_api.cpp
 * Copyright 2008-2021 LibRaw LLC (info@libraw.org)
 * Created: Sat Mar  8 , 2008
 *
 * LibRaw C interface


LibRaw is free software; you can redistribute it and/or modify
it under the terms of the one of two licenses as you choose:

1. GNU LESSER GENERAL PUBLIC LICENSE version 2.1
   (See file LICENSE.LGPL provided in LibRaw distribution archive for details).

2. COMMON DEVELOPMENT AND DISTRIBUTION LICENSE (CDDL) Version 1.0
   (See file LICENSE.CDDL provided in LibRaw distribution archive for details).

 */

#include <math.h>
#include <errno.h>
#include "libraw/libraw.h"

#ifdef __cplusplus
#include <new>
extern "C"
{
#endif

  libraw_data_t *libraw_init(unsigned int flags)
  {
    LibRaw *ret;
    try
    {
      ret = new LibRaw(flags);
    }
    catch (const std::bad_alloc& )
    {
      return NULL;
    }
    return &(ret->imgdata);
  }

  unsigned libraw_capabilities() { return LibRaw::capabilities(); }
  const char *libraw_version() { return LibRaw::version(); }
  const char *libraw_strprogress(enum LibRaw_progress p)
  {
    return LibRaw::strprogress(p);
  }
  int libraw_versionNumber() { return LibRaw::versionNumber(); }
  const char **libraw_cameraList() { return LibRaw::cameraList(); }
  int libraw_cameraCount() { return LibRaw::cameraCount(); }
  const char *libraw_unpack_function_name(libraw_data_t *lr)
  {
    if (!lr)
      return "NULL parameter passed";
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->unpack_function_name();
  }

  void libraw_subtract_black(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->subtract_black();
  }

  int libraw_open_file(libraw_data_t *lr, const char *file)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_file(file);
  }

  class CWrapperDataStream : public LibRaw_abstract_datastream {
    libraw_abstract_datastream_t stream;

  public:
    CWrapperDataStream(libraw_abstract_datastream_t* s) : stream(*s) {}

    int valid()
    {
      return stream.valid ? stream.valid(stream.userptr) : true;
    }

    int read(void *dst, size_t len, size_t cnt)
    {
      if (!stream.read) return -1;
      return stream.read(dst, len, cnt, stream.userptr);
    }

    int seek(INT64 pos, int whence)
    {
      if (!stream.seek) return -1;
      return stream.seek(pos, whence, stream.userptr);
    }

    INT64 tell()
    {
      if (!stream.tell) return -1;
      return stream.tell(stream.userptr);
    }

    INT64 size()
    {
      if (!stream.size) return -1;
      return stream.size(stream.userptr);
    }

    int get_char()
    {
      if (!stream.get_char) return -1;
      return stream.get_char(stream.userptr);
    }

    char *gets(char *s, int sz)
    {
      if (!stream.gets) return NULL;
      return stream.gets(s, sz, stream.userptr);
    }

    int scanf_one(const char *fmt, void *arg)
    {
      if (!stream.scanf_one) return -1;
      return stream.scanf_one(fmt, arg, stream.userptr);
    }

    int eof()
    {
      if (!stream.size) return -1;
      return stream.eof(stream.userptr);
    }

#ifdef LIBRAW_OLD_VIDEO_SUPPORT
	void *make_jas_stream()
    {
      if (!stream.make_jas_stream) return NULL;
      return stream.make_jas_stream(stream.userptr);
    }
#endif
  };


  int libraw_open_datastream(libraw_data_t *lr, libraw_abstract_datastream_t *stream)
  {
    auto s = new CWrapperDataStream(stream);
    return ((LibRaw*)lr->parent_class)->open_datastream(s);
  }


  static int ifp_wrap_valid(void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->valid();
  }
  static int ifp_wrap_read(void *dst, size_t sz, size_t cnt, void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->read(dst, sz, cnt);
  }
  static int ifp_wrap_seek(INT64 pos, int whence, void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->seek(pos, whence);
  }
  static INT64 ifp_wrap_tell(void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->tell();
  }
  static INT64 ifp_wrap_size(void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->size();
  }
  static int ifp_wrap_get_char(void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->get_char();
  }
  static char *ifp_wrap_gets(char *s, int sz, void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->gets(s, sz);
  }
  static int ifp_wrap_scanf_one(const char *fmt, void *arg, void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->scanf_one(fmt, arg);
  }
  static int ifp_wrap_eof(void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->eof();
  }
#ifdef LIBRAW_OLD_VIDEO_SUPPORT
  static void *ifp_wrap_make_jas_stream(void *userptr){
    return ((LibRaw_abstract_datastream*)userptr)->make_jas_stream();
  }
#endif

  void libraw_wrap_ifp_stream(void *ifp, libraw_abstract_datastream_t *dst)
  {
    dst->userptr = ifp;
    dst->valid = &ifp_wrap_valid;
    dst->read = &ifp_wrap_read;
    dst->seek = &ifp_wrap_seek;
    dst->tell = &ifp_wrap_tell;
    dst->size = &ifp_wrap_size;
    dst->get_char = &ifp_wrap_get_char;
    dst->gets = &ifp_wrap_gets;
    dst->scanf_one = &ifp_wrap_scanf_one;
    dst->eof = &ifp_wrap_eof;
#ifdef LIBRAW_OLD_VIDEO_SUPPORT
	dst->make_jas_stream = &ifp_wrap_make_jas_stream;
#endif
  }

  libraw_iparams_t *libraw_get_iparams(libraw_data_t *lr)
  {
    if (!lr)
      return NULL;
    return &(lr->idata);
  }

  libraw_lensinfo_t *libraw_get_lensinfo(libraw_data_t *lr)
  {
    if (!lr)
      return NULL;
    return &(lr->lens);
  }

  libraw_imgother_t *libraw_get_imgother(libraw_data_t *lr)
  {
    if (!lr)
      return NULL;
    return &(lr->other);
  }

#ifndef LIBRAW_NO_IOSTREAMS_DATASTREAM
  int libraw_open_file_ex(libraw_data_t *lr, const char *file, INT64 sz)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_file(file, sz);
  }
#endif

#ifdef LIBRAW_WIN32_UNICODEPATHS
  int libraw_open_wfile(libraw_data_t *lr, const wchar_t *file)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_file(file);
  }

#ifndef LIBRAW_NO_IOSTREAMS_DATASTREAM
  int libraw_open_wfile_ex(libraw_data_t *lr, const wchar_t *file, INT64 sz)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_file(file, sz);
  }
#endif
#endif
  int libraw_open_buffer(libraw_data_t *lr, const void *buffer, size_t size)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_buffer(buffer, size);
  }
  int libraw_open_bayer(libraw_data_t *lr, unsigned char *data,
                        unsigned datalen, ushort _raw_width, ushort _raw_height,
                        ushort _left_margin, ushort _top_margin,
                        ushort _right_margin, ushort _bottom_margin,
                        unsigned char procflags, unsigned char bayer_pattern,
                        unsigned unused_bits, unsigned otherflags,
                        unsigned black_level)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->open_bayer(data, datalen, _raw_width, _raw_height, _left_margin,
                          _top_margin, _right_margin, _bottom_margin, procflags,
                          bayer_pattern, unused_bits, otherflags, black_level);
  }
  int libraw_unpack(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->unpack();
  }
  int libraw_unpack_thumb(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->unpack_thumb();
  }
  void libraw_recycle_datastream(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->recycle_datastream();
  }
  void libraw_recycle(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->recycle();
  }
  void libraw_close(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    delete ip;
  }

  void libraw_set_exifparser_handler(libraw_data_t *lr, exif_parser_callback cb,
                                     void *data)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->set_exifparser_handler(cb, data);
  }

  void libraw_set_memerror_handler(libraw_data_t *lr, memory_callback cb,
                                   void *data)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->set_memerror_handler(cb, data);
  }
  void libraw_set_dataerror_handler(libraw_data_t *lr, data_callback func,
                                    void *data)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->set_dataerror_handler(func, data);
  }
  void libraw_set_progress_handler(libraw_data_t *lr, progress_callback cb,
                                   void *data)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->set_progress_handler(cb, data);
  }

  // DCRAW
  int libraw_adjust_sizes_info_only(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->adjust_sizes_info_only();
  }
  int libraw_dcraw_ppm_tiff_writer(libraw_data_t *lr, const char *filename)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_ppm_tiff_writer(filename);
  }
  int libraw_dcraw_thumb_writer(libraw_data_t *lr, const char *fname)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_thumb_writer(fname);
  }
  int libraw_dcraw_process(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_process();
  }
  libraw_processed_image_t *libraw_dcraw_make_mem_image(libraw_data_t *lr,
                                                        int *errc)
  {
    if (!lr)
    {
      if (errc)
        *errc = EINVAL;
      return NULL;
    }
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_make_mem_image(errc);
  }
  libraw_processed_image_t *libraw_dcraw_make_mem_thumb(libraw_data_t *lr,
                                                        int *errc)
  {
    if (!lr)
    {
      if (errc)
        *errc = EINVAL;
      return NULL;
    }
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_make_mem_thumb(errc);
  }

  void libraw_dcraw_clear_mem(libraw_processed_image_t *p)
  {
    LibRaw::dcraw_clear_mem(p);
  }

  int libraw_raw2image(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->raw2image();
  }
  void libraw_free_image(libraw_data_t *lr)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->free_image();
  }
  int libraw_get_decoder_info(libraw_data_t *lr, libraw_decoder_info_t *d)
  {
    if (!lr || !d)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->get_decoder_info(d);
  }
  int libraw_COLOR(libraw_data_t *lr, int row, int col)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->COLOR(row, col);
  }

  /* getters/setters used by 3DLut Creator */
  DllDef void libraw_set_demosaic(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.user_qual = value;
  }

  DllDef void libraw_set_output_color(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.output_color = value;
  }

  DllDef void libraw_set_adjust_maximum_thr(libraw_data_t *lr, float value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.adjust_maximum_thr = value;
  }

  DllDef void libraw_set_output_bps(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.output_bps = value;
  }

  	DllDef void libraw_set_output_tif(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.output_tiff = value;
  }

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define LIM(x, min, max) MAX(min, MIN(x, max))

  DllDef void libraw_set_user_mul(libraw_data_t *lr, int index, float val)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.user_mul[LIM(index, 0, 3)] = val;
  }

  DllDef void libraw_set_gamma(libraw_data_t *lr, int index, float value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.gamm[LIM(index, 0, 5)] = value;
  }

  DllDef void libraw_set_no_auto_bright(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.no_auto_bright = value;
  }

  DllDef void libraw_set_bright(libraw_data_t *lr, float value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.bright = value;
  }

  DllDef void libraw_set_highlight(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.highlight = value;
  }

  DllDef void libraw_set_fbdd_noiserd(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.fbdd_noiserd = value;
  }

  DllDef int libraw_get_raw_height(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->sizes.raw_height;
  }

  DllDef int libraw_get_raw_width(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->sizes.raw_width;
  }

  DllDef int libraw_get_iheight(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->sizes.iheight;
  }

  DllDef int libraw_get_iwidth(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->sizes.iwidth;
  }

  DllDef float libraw_get_cam_mul(libraw_data_t *lr, int index)
  {
    if (!lr)
      return EINVAL;
    return lr->color.cam_mul[LIM(index, 0, 3)];
  }

  DllDef float libraw_get_pre_mul(libraw_data_t *lr, int index)
  {
    if (!lr)
      return EINVAL;
    return lr->color.pre_mul[LIM(index, 0, 3)];
  }

  DllDef float libraw_get_rgb_cam(libraw_data_t *lr, int index1, int index2)
  {
    if (!lr)
      return EINVAL;
    return lr->color.rgb_cam[LIM(index1, 0, 2)][LIM(index2, 0, 3)];
  }

  DllDef int libraw_get_color_maximum(libraw_data_t *lr)
  {
    if (!lr)
      return EINVAL;
    return lr->color.maximum;
  }

#ifdef __cplusplus
}
#endif
