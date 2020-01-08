/*
 * Copyright (c) 2011-2019, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <media/msm_media_info.h>

#include <drm/drm_fourcc.h>

#include <algorithm>

#include "gr_adreno_info.h"
#include "gr_utils.h"

#define ASTC_BLOCK_SIZE 16

#ifndef COLOR_FMT_P010_UBWC
#define COLOR_FMT_P010_UBWC 9
#endif

namespace gralloc {

bool IsYuvFormat(int format) {
  switch (format) {
    case HAL_PIXEL_FORMAT_YCbCr_420_SP:
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:  // Same as YCbCr_420_SP_VENUS
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_YCrCb_422_SP:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_ADRENO:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV21_ZSL:
    case HAL_PIXEL_FORMAT_RAW16:
    case HAL_PIXEL_FORMAT_Y16:
    case HAL_PIXEL_FORMAT_RAW12:
    case HAL_PIXEL_FORMAT_RAW10:
    case HAL_PIXEL_FORMAT_YV12:
    case HAL_PIXEL_FORMAT_Y8:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010:
    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
    // Below formats used by camera and VR
    case HAL_PIXEL_FORMAT_BLOB:
    case HAL_PIXEL_FORMAT_RAW_OPAQUE:
      return true;
    default:
      return false;
  }
}

bool IsUncompressedRGBFormat(int format) {
  switch (format) {
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_RGB_888:
    case HAL_PIXEL_FORMAT_RGB_565:
    case HAL_PIXEL_FORMAT_BGR_565:
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_RGBA_5551:
    case HAL_PIXEL_FORMAT_RGBA_4444:
    case HAL_PIXEL_FORMAT_R_8:
    case HAL_PIXEL_FORMAT_RG_88:
    case HAL_PIXEL_FORMAT_BGRX_8888:
    case HAL_PIXEL_FORMAT_RGBA_1010102:
    case HAL_PIXEL_FORMAT_ARGB_2101010:
    case HAL_PIXEL_FORMAT_RGBX_1010102:
    case HAL_PIXEL_FORMAT_XRGB_2101010:
    case HAL_PIXEL_FORMAT_BGRA_1010102:
    case HAL_PIXEL_FORMAT_ABGR_2101010:
    case HAL_PIXEL_FORMAT_BGRX_1010102:
    case HAL_PIXEL_FORMAT_XBGR_2101010:
    case HAL_PIXEL_FORMAT_RGBA_FP16:
    case HAL_PIXEL_FORMAT_BGR_888:
      return true;
    default:
      break;
  }

  return false;
}

bool IsCompressedRGBFormat(int format) {
  switch (format) {
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_5x4_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_5x5_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_6x5_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_6x6_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_8x5_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_8x6_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_8x8_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_10x5_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_10x6_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_10x8_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_10x10_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_12x10_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_12x12_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
      return true;
    default:
      break;
  }

  return false;
}

uint32_t GetBppForUncompressedRGB(int format) {
  uint32_t bpp = 0;
  switch (format) {
    case HAL_PIXEL_FORMAT_RGBA_FP16:
      bpp = 8;
      break;
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_BGRX_8888:
    case HAL_PIXEL_FORMAT_RGBA_1010102:
    case HAL_PIXEL_FORMAT_ARGB_2101010:
    case HAL_PIXEL_FORMAT_RGBX_1010102:
    case HAL_PIXEL_FORMAT_XRGB_2101010:
    case HAL_PIXEL_FORMAT_BGRA_1010102:
    case HAL_PIXEL_FORMAT_ABGR_2101010:
    case HAL_PIXEL_FORMAT_BGRX_1010102:
    case HAL_PIXEL_FORMAT_XBGR_2101010:
      bpp = 4;
      break;
    case HAL_PIXEL_FORMAT_RGB_888:
    case HAL_PIXEL_FORMAT_BGR_888:
      bpp = 3;
      break;
    case HAL_PIXEL_FORMAT_RGB_565:
    case HAL_PIXEL_FORMAT_BGR_565:
    case HAL_PIXEL_FORMAT_RGBA_5551:
    case HAL_PIXEL_FORMAT_RGBA_4444:
      bpp = 2;
      break;
    default:
      ALOGE("Error : %s New format request = 0x%x", __FUNCTION__, format);
      break;
  }

  return bpp;
}

bool CpuCanAccess(uint64_t usage) {
  return CpuCanRead(usage) || CpuCanWrite(usage);
}

bool CpuCanRead(uint64_t usage) {
  if (usage & BufferUsage::CPU_READ_MASK) {
    return true;
  }

  return false;
}

bool CpuCanWrite(uint64_t usage) {
  if (usage & BufferUsage::CPU_WRITE_MASK) {
    // Application intends to use CPU for rendering
    return true;
  }

  return false;
}

uint32_t GetDataAlignment(int format, uint64_t usage) {
  uint32_t align = UINT(getpagesize());
  if (format == HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED) {
    align = 8192;
  }

  if (usage & BufferUsage::PROTECTED) {
    if ((usage & BufferUsage::CAMERA_OUTPUT) || (usage & GRALLOC_USAGE_PRIVATE_SECURE_DISPLAY)) {
      // The alignment here reflects qsee mmu V7L/V8L requirement
      align = SZ_2M;
    } else {
      align = SECURE_ALIGN;
    }
  }

  return align;
}

bool IsGPUFlagSupported(uint64_t usage) {
  bool ret = true;
  if ((usage & BufferUsage::GPU_MIPMAP_COMPLETE)) {
    ALOGE("GPU_MIPMAP_COMPLETE not supported");
    ret = false;
  }

  if ((usage & BufferUsage::GPU_CUBE_MAP)) {
    ALOGE("GPU_CUBE_MAP not supported");
    ret = false;
  }

  return ret;
}

int GetBpp(int format) {
  if (IsUncompressedRGBFormat(format)) {
    return GetBppForUncompressedRGB(format);
  }
  switch (format) {
    case HAL_PIXEL_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR:
    case HAL_PIXEL_FORMAT_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case HAL_PIXEL_FORMAT_RAW8:
    case HAL_PIXEL_FORMAT_Y8:
      return 1;
    case HAL_PIXEL_FORMAT_RAW16:
    case HAL_PIXEL_FORMAT_Y16:
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
    case HAL_PIXEL_FORMAT_YCrCb_422_SP:
    case HAL_PIXEL_FORMAT_YCbCr_422_I:
    case HAL_PIXEL_FORMAT_YCrCb_422_I:
    case HAL_PIXEL_FORMAT_CbYCrY_422_I:
      return 2;
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010:
      return 3;
    default:
      return -1;
  }
}

// Returns the final buffer size meant to be allocated with ion
unsigned int GetSize(const BufferInfo &info, unsigned int alignedw, unsigned int alignedh) {
  unsigned int size = 0;
  int format = info.format;
  int width = info.width;
  int height = info.height;
  uint64_t usage = info.usage;

  if (!IsGPUFlagSupported(usage)) {
    ALOGE("Unsupported GPU usage flags present 0x%" PRIx64, usage);
    return 0;
  }

  if (IsUBwcEnabled(format, usage)) {
    size = GetUBwcSize(width, height, format, alignedw, alignedh);
  } else if (IsUncompressedRGBFormat(format)) {
    uint32_t bpp = GetBppForUncompressedRGB(format);
    size = alignedw * alignedh * bpp;
  } else if (IsCompressedRGBFormat(format)) {
    size = alignedw * alignedh * ASTC_BLOCK_SIZE;
  } else {

    // Below switch should be for only YUV/custom formats
    switch (format) {
      case HAL_PIXEL_FORMAT_RAW16:
      case HAL_PIXEL_FORMAT_Y16:size = alignedw * alignedh * 2;
        break;
      case HAL_PIXEL_FORMAT_RAW10:
      case HAL_PIXEL_FORMAT_RAW12:size = ALIGN(alignedw * alignedh, SIZE_4K);
        break;
      case HAL_PIXEL_FORMAT_RAW8:
      case HAL_PIXEL_FORMAT_Y8:size = alignedw * alignedh * 1;
        break;
        // adreno formats
      case HAL_PIXEL_FORMAT_YCrCb_420_SP_ADRENO:  // NV21
        size = ALIGN(alignedw * alignedh, SIZE_4K);
        size += (unsigned int) ALIGN(2 * ALIGN(width / 2, 32) * ALIGN(height / 2, 32), SIZE_4K);
        break;
      case HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED:  // NV12
        // The chroma plane is subsampled,
        // but the pitch in bytes is unchanged
        // The GPU needs 4K alignment, but the video decoder needs 8K
        size = ALIGN(alignedw * alignedh, SIZE_8K);
        size += ALIGN(alignedw * (unsigned int) ALIGN(height / 2, 32), SIZE_8K);
        break;
      case HAL_PIXEL_FORMAT_YV12:
        if ((format == HAL_PIXEL_FORMAT_YV12) && ((width & 1) || (height & 1))) {
          ALOGE("w or h is odd for the YV12 format");
          return 0;
        }
        size = alignedw * alignedh + (ALIGN(alignedw / 2, 16) * (alignedh / 2)) * 2;
        size = ALIGN(size, (unsigned int) SIZE_4K);
        break;
      case HAL_PIXEL_FORMAT_YCbCr_420_SP:
      case HAL_PIXEL_FORMAT_YCrCb_420_SP:
        size = ALIGN((alignedw * alignedh) + (alignedw * alignedh) / 2 + 1, SIZE_4K);
        break;
      case HAL_PIXEL_FORMAT_YCbCr_420_P010:
        size = ALIGN((alignedw * alignedh * 2) + (alignedw * alignedh) + 1, SIZE_4K);
        break;
      case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
        size = VENUS_BUFFER_SIZE(COLOR_FMT_P010,
                                 width,
                                 height);
        break;
      case HAL_PIXEL_FORMAT_YCbCr_422_SP:
      case HAL_PIXEL_FORMAT_YCrCb_422_SP:
      case HAL_PIXEL_FORMAT_YCbCr_422_I:
      case HAL_PIXEL_FORMAT_YCrCb_422_I:
      case HAL_PIXEL_FORMAT_CbYCrY_422_I:
        if (width & 1) {
          ALOGE("width is odd for the YUV422_SP format");
          return 0;
        }
        size = ALIGN(alignedw * alignedh * 2, SIZE_4K);
        break;
      case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
      case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:size = VENUS_BUFFER_SIZE(COLOR_FMT_NV12, width, height);
        break;
      case HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS:
        size = VENUS_BUFFER_SIZE(COLOR_FMT_NV21, width, height);
        break;
      case HAL_PIXEL_FORMAT_BLOB:
      case HAL_PIXEL_FORMAT_RAW_OPAQUE:
        if (height != 1) {
          ALOGE("%s: Buffers with HAL_PIXEL_FORMAT_BLOB must have height 1 ", __FUNCTION__);
          return 0;
        }
        size = (unsigned int) width;
        break;
      case HAL_PIXEL_FORMAT_NV21_ZSL:
        size = ALIGN((alignedw * alignedh) + (alignedw * alignedh) / 2,
                     SIZE_4K);
        break;
      default:ALOGE("%s: Unrecognized pixel format: 0x%x", __FUNCTION__, format);
        return 0;
    }
  }
  auto align = GetDataAlignment(format, usage);
  size = ALIGN(size, align) * info.layer_count;
  return size;
}

void GetBufferSizeAndDimensions(const BufferInfo &info, unsigned int *size, unsigned int *alignedw,
                                unsigned int *alignedh) {
  GraphicsMetadata graphics_metadata = {};
  GetBufferSizeAndDimensions(info, size, alignedw, alignedh, &graphics_metadata);
}

void GetBufferSizeAndDimensions(const BufferInfo &info, unsigned int *size, unsigned int *alignedw,
                                unsigned int *alignedh, GraphicsMetadata *graphics_metadata) {
  int buffer_type = GetBufferType(info.format);
  if (CanUseAdrenoForSize(buffer_type, info.usage)) {
    GetGpuResourceSizeAndDimensions(info, size, alignedw, alignedh, graphics_metadata);
  } else {
    GetAlignedWidthAndHeight(info, alignedw, alignedh);
    *size = GetSize(info, *alignedw, *alignedh);
  }
}

void GetYuvUbwcSPPlaneInfo(uint64_t base, uint32_t width, uint32_t height, int color_format,
                           struct android_ycbcr *ycbcr) {
  // UBWC buffer has these 4 planes in the following sequence:
  // Y_Meta_Plane, Y_Plane, UV_Meta_Plane, UV_Plane
  unsigned int y_meta_stride, y_meta_height, y_meta_size;
  unsigned int y_stride, y_height, y_size;
  unsigned int c_meta_stride, c_meta_height, c_meta_size;
  unsigned int alignment = 4096;

  y_meta_stride = VENUS_Y_META_STRIDE(color_format, INT(width));
  y_meta_height = VENUS_Y_META_SCANLINES(color_format, INT(height));
  y_meta_size = ALIGN((y_meta_stride * y_meta_height), alignment);

  y_stride = VENUS_Y_STRIDE(color_format, INT(width));
  y_height = VENUS_Y_SCANLINES(color_format, INT(height));
  y_size = ALIGN((y_stride * y_height), alignment);

  c_meta_stride = VENUS_UV_META_STRIDE(color_format, INT(width));
  c_meta_height = VENUS_UV_META_SCANLINES(color_format, INT(height));
  c_meta_size = ALIGN((c_meta_stride * c_meta_height), alignment);

  ycbcr->y = reinterpret_cast<void *>(base + y_meta_size);
  ycbcr->cb = reinterpret_cast<void *>(base + y_meta_size + y_size + c_meta_size);
  ycbcr->cr = reinterpret_cast<void *>(base + y_meta_size + y_size + c_meta_size + 1);
  ycbcr->ystride = y_stride;
  ycbcr->cstride = VENUS_UV_STRIDE(color_format, INT(width));
}

void GetYuvUbwcInterlacedSPPlaneInfo(uint64_t base, uint32_t width, uint32_t height,
                                     int color_format, struct android_ycbcr ycbcr[2]) {
  unsigned int uv_stride, uv_height, uv_size;
  unsigned int alignment = 4096;
  uint64_t field_base;

  // UBWC interlaced has top-bottom field layout with each field as
  // 4-plane NV12_UBWC with width = image_width & height = image_height / 2.
  // Client passed ycbcr argument is ptr to struct android_ycbcr[2].
  // Plane info to be filled for each field separately.
  height = (height + 1) >> 1;
  uv_stride = VENUS_UV_STRIDE(color_format, INT(width));
  uv_height = VENUS_UV_SCANLINES(color_format, INT(height));
  uv_size = ALIGN((uv_stride * uv_height), alignment);

  field_base = base;
  GetYuvUbwcSPPlaneInfo(field_base, width, height, COLOR_FMT_NV12_UBWC, &ycbcr[0]);

  memset(ycbcr[1].reserved, 0, sizeof(ycbcr[1].reserved));
  field_base = reinterpret_cast<uint64_t>(ycbcr[0].cb) + uv_size;
  GetYuvUbwcSPPlaneInfo(field_base, width, height, COLOR_FMT_NV12_UBWC, &ycbcr[1]);
}

void GetYuvSPPlaneInfo(uint64_t base, uint32_t width, uint32_t height, uint32_t bpp,
                       struct android_ycbcr *ycbcr) {
  unsigned int ystride, cstride;

  ystride = cstride = UINT(width) * bpp;
  ycbcr->y = reinterpret_cast<void *>(base);
  ycbcr->cb = reinterpret_cast<void *>(base + ystride * UINT(height));
  ycbcr->cr = reinterpret_cast<void *>(base + ystride * UINT(height) + 1);
  ycbcr->ystride = ystride;
  ycbcr->cstride = cstride;
  ycbcr->chroma_step = 2 * bpp;
}

int GetYUVPlaneInfo(const private_handle_t *hnd, struct android_ycbcr ycbcr[2]) {
  int err = 0;
  uint32_t width = UINT(hnd->width);
  uint32_t height = UINT(hnd->height);
  int format = hnd->format;
  uint64_t usage = hnd->usage;
  unsigned int ystride, cstride;
  bool interlaced = false;

  memset(ycbcr->reserved, 0, sizeof(ycbcr->reserved));

  // Check if UBWC buffer has been rendered in linear format.
  int linear_format = 0;
  if (getMetaData(const_cast<private_handle_t *>(hnd), GET_LINEAR_FORMAT, &linear_format) == 0) {
    format = INT(linear_format);
  }

  // Check metadata if the geometry has been updated.
  BufferDim_t buffer_dim;
  if (getMetaData(const_cast<private_handle_t *>(hnd), GET_BUFFER_GEOMETRY, &buffer_dim) == 0) {
    BufferInfo info(buffer_dim.sliceWidth, buffer_dim.sliceHeight, format, usage);
    GetAlignedWidthAndHeight(info, &width, &height);
  }

  // Check metadata for interlaced content.
  int interlace_flag = 0;
  if (getMetaData(const_cast<private_handle_t *>(hnd), GET_PP_PARAM_INTERLACED, &interlace_flag) ==
      0) {
    interlaced = interlace_flag;
  }

  // Get the chroma offsets from the handle width/height. We take advantage
  // of the fact the width _is_ the stride
  switch (format) {
    // Semiplanar
    case HAL_PIXEL_FORMAT_YCbCr_420_SP:
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:
      // Same as YCbCr_420_SP_VENUS
      GetYuvSPPlaneInfo(hnd->base, width, height, 1, ycbcr);
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
      if (!interlaced) {
        GetYuvUbwcSPPlaneInfo(hnd->base, width, height, COLOR_FMT_NV12_UBWC, ycbcr);
      } else {
        GetYuvUbwcInterlacedSPPlaneInfo(hnd->base, width, height, COLOR_FMT_NV12_UBWC, ycbcr);
      }
      ycbcr->chroma_step = 2;
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_P010:
      GetYuvSPPlaneInfo(hnd->base, width, height, 2, ycbcr);
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
      GetYuvUbwcSPPlaneInfo(hnd->base, width, height, COLOR_FMT_NV12_BPP10_UBWC, ycbcr);
      ycbcr->chroma_step = 3;
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
      GetYuvUbwcSPPlaneInfo(hnd->base, width, height, COLOR_FMT_P010_UBWC, ycbcr);
      ycbcr->chroma_step = 4;
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
      ystride = VENUS_Y_STRIDE(COLOR_FMT_P010, width);
      cstride = VENUS_UV_STRIDE(COLOR_FMT_P010, width);
      ycbcr->y = reinterpret_cast<void *>(hnd->base);
      ycbcr->cb =
          reinterpret_cast<void *>(hnd->base + ystride * VENUS_Y_SCANLINES(COLOR_FMT_P010, height));
      ycbcr->cr = reinterpret_cast<void *>(hnd->base +
                                           ystride * VENUS_Y_SCANLINES(COLOR_FMT_P010, height) + 1);
      ycbcr->ystride = ystride;
      ycbcr->cstride = cstride;
      ycbcr->chroma_step = 4;
      break;

    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_YCrCb_422_SP:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_ADRENO:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
    case HAL_PIXEL_FORMAT_NV21_ZSL:
    case HAL_PIXEL_FORMAT_RAW16:
    case HAL_PIXEL_FORMAT_Y16:
    case HAL_PIXEL_FORMAT_RAW10:
    case HAL_PIXEL_FORMAT_RAW8:
    case HAL_PIXEL_FORMAT_Y8:
      GetYuvSPPlaneInfo(hnd->base, width, height, 1, ycbcr);
      std::swap(ycbcr->cb, ycbcr->cr);
      break;

      // Planar
    case HAL_PIXEL_FORMAT_YV12:
      ystride = width;
      cstride = ALIGN(width / 2, 16);
      ycbcr->y = reinterpret_cast<void *>(hnd->base);
      ycbcr->cr = reinterpret_cast<void *>(hnd->base + ystride * height);
      ycbcr->cb = reinterpret_cast<void *>(hnd->base + ystride * height + cstride * height / 2);
      ycbcr->ystride = ystride;
      ycbcr->cstride = cstride;
      ycbcr->chroma_step = 1;
      break;
    case HAL_PIXEL_FORMAT_CbYCrY_422_I:
      ystride = width * 2;
      cstride = 0;
      ycbcr->y = reinterpret_cast<void *>(hnd->base);
      ycbcr->cr = NULL;
      ycbcr->cb = NULL;
      ycbcr->ystride = ystride;
      ycbcr->cstride = 0;
      ycbcr->chroma_step = 0;
      break;
      // Unsupported formats
    case HAL_PIXEL_FORMAT_YCbCr_422_I:
    case HAL_PIXEL_FORMAT_YCrCb_422_I:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED:
    default:
      ALOGD("%s: Invalid format passed: 0x%x", __FUNCTION__, format);
      err = -EINVAL;
  }

  return err;
}

// Explicitly defined UBWC formats
bool IsUBwcFormat(int format) {
  switch (format) {
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
      return true;
    default:
      return false;
  }
}

bool IsUBwcSupported(int format) {
  // Existing HAL formats with UBWC support
  switch (format) {
    case HAL_PIXEL_FORMAT_BGR_565:
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_RGBA_1010102:
    case HAL_PIXEL_FORMAT_RGBX_1010102:
    case HAL_PIXEL_FORMAT_DEPTH_16:
    case HAL_PIXEL_FORMAT_DEPTH_24:
    case HAL_PIXEL_FORMAT_DEPTH_24_STENCIL_8:
    case HAL_PIXEL_FORMAT_DEPTH_32F:
    case HAL_PIXEL_FORMAT_STENCIL_8:
      return true;
    default:
      break;
  }

  return false;
}

bool IsUBwcEnabled(int format, uint64_t usage) {
  // Allow UBWC, if client is using an explicitly defined UBWC pixel format.
  if (IsUBwcFormat(format)) {
    return true;
  }

  // Allow UBWC, if an OpenGL client sets UBWC usage flag and GPU plus MDP
  // support the format. OR if a non-OpenGL client like Rotator, sets UBWC
  // usage flag and MDP supports the format.
  if (((usage & GRALLOC_USAGE_PRIVATE_ALLOC_UBWC) ||
      (usage & BufferUsage::COMPOSER_CLIENT_TARGET))
      && IsUBwcSupported(format)) {
    bool enable = true;
    // Query GPU for UBWC only if buffer is intended to be used by GPU.
    if ((usage & BufferUsage::GPU_TEXTURE) || (usage & BufferUsage::GPU_RENDER_TARGET)) {
      if (AdrenoMemInfo::GetInstance()) {
        enable = AdrenoMemInfo::GetInstance()->IsUBWCSupportedByGPU(format);
      }
    }

    // Allow UBWC, only if CPU usage flags are not set
    if (enable && !(CpuCanAccess(usage))) {
      return true;
    }
  }

  return false;
}

void GetYuvUBwcWidthAndHeight(int width, int height, int format, unsigned int *aligned_w,
                              unsigned int *aligned_h) {
  switch (format) {
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
      *aligned_w = VENUS_Y_STRIDE(COLOR_FMT_NV12, width);
      *aligned_h = VENUS_Y_SCANLINES(COLOR_FMT_NV12, height);
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
      *aligned_w = VENUS_Y_STRIDE(COLOR_FMT_NV12_UBWC, width);
      *aligned_h = VENUS_Y_SCANLINES(COLOR_FMT_NV12_UBWC, height);
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
      // The macro returns the stride which is 4/3 times the width, hence * 3/4
      *aligned_w = (VENUS_Y_STRIDE(COLOR_FMT_NV12_BPP10_UBWC, width) * 3) / 4;
      *aligned_h = VENUS_Y_SCANLINES(COLOR_FMT_NV12_BPP10_UBWC, height);
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
      // The macro returns the stride which is 2 times the width, hence / 2
      *aligned_w = (VENUS_Y_STRIDE(COLOR_FMT_P010_UBWC, width) / 2);
      *aligned_h = VENUS_Y_SCANLINES(COLOR_FMT_P010_UBWC, height);
      break;
    default:
      ALOGE("%s: Unsupported pixel format: 0x%x", __FUNCTION__, format);
      *aligned_w = 0;
      *aligned_h = 0;
      break;
  }
}

void GetRgbUBwcBlockSize(uint32_t bpp, int *block_width, int *block_height) {
  *block_width = 0;
  *block_height = 0;

  switch (bpp) {
    case 2:
    case 4:
      *block_width = 16;
      *block_height = 4;
      break;
    case 8:
      *block_width = 8;
      *block_height = 4;
      break;
    case 16:
      *block_width = 4;
      *block_height = 4;
      break;
    default:
      ALOGE("%s: Unsupported bpp: %d", __FUNCTION__, bpp);
      break;
  }
}

unsigned int GetRgbUBwcMetaBufferSize(int width, int height, uint32_t bpp) {
  unsigned int size = 0;
  int meta_width, meta_height;
  int block_width, block_height;

  GetRgbUBwcBlockSize(bpp, &block_width, &block_height);
  if (!block_width || !block_height) {
    ALOGE("%s: Unsupported bpp: %d", __FUNCTION__, bpp);
    return size;
  }

  // Align meta buffer height to 16 blocks
  meta_height = ALIGN(((height + block_height - 1) / block_height), 16);

  // Align meta buffer width to 64 blocks
  meta_width = ALIGN(((width + block_width - 1) / block_width), 64);

  // Align meta buffer size to 4K
  size = (unsigned int)ALIGN((meta_width * meta_height), 4096);

  return size;
}

unsigned int GetUBwcSize(int width, int height, int format, unsigned int alignedw,
                         unsigned int alignedh) {
  unsigned int size = 0;
  uint32_t bpp = 0;
  switch (format) {
    case HAL_PIXEL_FORMAT_BGR_565:
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_RGBA_1010102:
    case HAL_PIXEL_FORMAT_RGBX_1010102:
      bpp = GetBppForUncompressedRGB(format);
      size = alignedw * alignedh * bpp;
      size += GetRgbUBwcMetaBufferSize(width, height, bpp);
      break;
    /*
     * 1. The CtsMediaV2TestCases#CodecEncoderSurfaceTest is a transcode use case and shares
     *    same surface between encoder and decoder.
     * 2. Configures encoder with Opaque color format thus encoder sets ubwc usage bits and
     *    is configured with NV12_UBWC format.
     * 3. Configures decoder as 'flexible', thus configuring decoder with NV12 format.
     * 4. Decoder should produce output to surface that will be fed back to encoder as input.
     * 5. Though UBWC is enabled, we need to compute the actual buffer size (including aligned
     *    width and height) based on pixel format that is set.
     */
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
      size = VENUS_BUFFER_SIZE(COLOR_FMT_NV12, width, height);
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
      size = VENUS_BUFFER_SIZE(COLOR_FMT_NV12_UBWC, width, height);
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
      size = VENUS_BUFFER_SIZE(COLOR_FMT_NV12_BPP10_UBWC, width, height);
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
      size = VENUS_BUFFER_SIZE(COLOR_FMT_P010_UBWC, width, height);
      break;
    default:
      ALOGE("%s: Unsupported pixel format: 0x%x", __FUNCTION__, format);
      break;
  }

  return size;
}

int GetRgbDataAddress(private_handle_t *hnd, void **rgb_data) {
  int err = 0;

  // This api is for RGB* formats
  if (!IsUncompressedRGBFormat(hnd->format)) {
    return -EINVAL;
  }

  // linear buffer, nothing to do further
  if (!(hnd->flags & private_handle_t::PRIV_FLAGS_UBWC_ALIGNED)) {
    *rgb_data = reinterpret_cast<void *>(hnd->base);
    return err;
  }

  unsigned int meta_size = 0;
  uint32_t bpp = GetBppForUncompressedRGB(hnd->format);
  switch (hnd->format) {
    case HAL_PIXEL_FORMAT_BGR_565:
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_RGBA_1010102:
    case HAL_PIXEL_FORMAT_RGBX_1010102:
      meta_size = GetRgbUBwcMetaBufferSize(hnd->width, hnd->height, bpp);
      break;
    default:
      ALOGE("%s:Unsupported RGB format: 0x%x", __FUNCTION__, hnd->format);
      err = -EINVAL;
      break;
  }
  *rgb_data = reinterpret_cast<void *>(hnd->base + meta_size);

  return err;
}

void GetCustomDimensions(private_handle_t *hnd, int *stride, int *height) {
  BufferDim_t buffer_dim;
  int interlaced = 0;

  *stride = hnd->width;
  *height = hnd->height;
  if (getMetaData(hnd, GET_BUFFER_GEOMETRY, &buffer_dim) == 0) {
    *stride = buffer_dim.sliceWidth;
    *height = buffer_dim.sliceHeight;
  } else if (getMetaData(hnd, GET_PP_PARAM_INTERLACED, &interlaced) == 0) {
    if (interlaced && IsUBwcFormat(hnd->format)) {
      unsigned int alignedw = 0, alignedh = 0;
      // Get re-aligned height for single ubwc interlaced field and
      // multiply by 2 to get frame height.
      BufferInfo info(hnd->width, ((hnd->height + 1) >> 1), hnd->format);
      GetAlignedWidthAndHeight(info, &alignedw, &alignedh);
      *stride = static_cast<int>(alignedw);
      *height = static_cast<int>(alignedh * 2);
    }
  }
}

void GetColorSpaceFromMetadata(private_handle_t *hnd, int *color_space) {
  ColorMetaData color_metadata;
  if (getMetaData(hnd, GET_COLOR_METADATA, &color_metadata) == 0) {
    switch (color_metadata.colorPrimaries) {
      case ColorPrimaries_BT709_5:
        *color_space = HAL_CSC_ITU_R_709;
        break;
      case ColorPrimaries_BT601_6_525:
      case ColorPrimaries_BT601_6_625:
        *color_space = ((color_metadata.range) ? HAL_CSC_ITU_R_601_FR : HAL_CSC_ITU_R_601);
        break;
      case ColorPrimaries_BT2020:
        *color_space = (color_metadata.range) ? HAL_CSC_ITU_R_2020_FR : HAL_CSC_ITU_R_2020;
        break;
      default:
        ALOGE("Unknown Color Space = %d", color_metadata.colorPrimaries);
        break;
    }
  } else if (getMetaData(hnd, GET_COLOR_SPACE, color_space) != 0) {
    *color_space = 0;
  }
}

void GetAlignedWidthAndHeight(const BufferInfo &info, unsigned int *alignedw,
                              unsigned int *alignedh) {
  int width = info.width;
  int height = info.height;
  int format = info.format;
  uint64_t usage = info.usage;

  // Currently surface padding is only computed for RGB* surfaces.
  bool ubwc_enabled = IsUBwcEnabled(format, usage);
  int tile = ubwc_enabled;

  if (IsUncompressedRGBFormat(format)) {
    if (AdrenoMemInfo::GetInstance()) {
      AdrenoMemInfo::GetInstance()->AlignUnCompressedRGB(width, height, format, tile, alignedw,
                                                         alignedh);
    }
    return;
  }

  if (ubwc_enabled) {
    GetYuvUBwcWidthAndHeight(width, height, format, alignedw, alignedh);
    return;
  }

  if (IsCompressedRGBFormat(format)) {
    if (AdrenoMemInfo::GetInstance()) {
      AdrenoMemInfo::GetInstance()->AlignCompressedRGB(width, height, format, alignedw, alignedh);
    }
    return;
  }

  int aligned_w = width;
  int aligned_h = height;
  unsigned int alignment = 32;

  // Below should be only YUV family
  switch (format) {
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
#ifdef USE_ALIGNED_YCRCB_HEIGHT
      aligned_h = ALIGN(height, 32);
#endif
#ifdef USE_UNALIGNED_YCRCB
      break;
#endif
    case HAL_PIXEL_FORMAT_YCbCr_420_SP:
      if (AdrenoMemInfo::GetInstance() == nullptr) {
        return;
      }
      alignment = AdrenoMemInfo::GetInstance()->GetGpuPixelAlignment();
#ifdef USE_ALIGNED_YCBCR_HEIGHT
      aligned_h = ALIGN(height, 32);
#endif
      aligned_w = ALIGN(width, alignment);
      break;
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_ADRENO:
      aligned_w = ALIGN(width, alignment);
      break;
    case HAL_PIXEL_FORMAT_RAW16:
    case HAL_PIXEL_FORMAT_Y16:
    case HAL_PIXEL_FORMAT_Y8:
      aligned_w = ALIGN(width, 16);
      break;
    case HAL_PIXEL_FORMAT_RAW12:
      aligned_w = ALIGN(width * 12 / 8, 16);
      break;
    case HAL_PIXEL_FORMAT_RAW10:
      aligned_w = ALIGN(width * 10 / 8, 16);
      break;
    case HAL_PIXEL_FORMAT_RAW8:
      aligned_w = ALIGN(width, 16);
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED:
      aligned_w = ALIGN(width, 128);
      break;
    case HAL_PIXEL_FORMAT_YV12:
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
    case HAL_PIXEL_FORMAT_YCrCb_422_SP:
    case HAL_PIXEL_FORMAT_YCbCr_422_I:
    case HAL_PIXEL_FORMAT_YCrCb_422_I:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010:
      aligned_w = ALIGN(width, 16);
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
      aligned_w = INT(VENUS_Y_STRIDE(COLOR_FMT_P010, width) / 2);
      aligned_h = INT(VENUS_Y_SCANLINES(COLOR_FMT_P010, height));
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:
      aligned_w = INT(VENUS_Y_STRIDE(COLOR_FMT_NV12, width));
      aligned_h = INT(VENUS_Y_SCANLINES(COLOR_FMT_NV12, height));
      break;
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
      aligned_w = INT(VENUS_Y_STRIDE(COLOR_FMT_NV21, width));
      aligned_h = INT(VENUS_Y_SCANLINES(COLOR_FMT_NV21, height));
      break;
    case HAL_PIXEL_FORMAT_BLOB:
    case HAL_PIXEL_FORMAT_RAW_OPAQUE:
      break;
    case HAL_PIXEL_FORMAT_NV21_ZSL:
#ifndef USE_UNALIGNED_NV21_ZSL
      aligned_w = ALIGN(width, 64);
      aligned_h = ALIGN(height, 64);
#endif
      break;
    default:
      break;
  }

  *alignedw = (unsigned int)aligned_w;
  *alignedh = (unsigned int)aligned_h;
}

int GetBufferLayout(private_handle_t *hnd, uint32_t stride[4], uint32_t offset[4],
                    uint32_t *num_planes) {
  if (!hnd || !stride || !offset || !num_planes) {
    return -EINVAL;
  }

  struct android_ycbcr yuvPlaneInfo[2] = {};
  *num_planes = 1;
  stride[0] = 0;

  switch (hnd->format) {
    case HAL_PIXEL_FORMAT_RGB_565:
    case HAL_PIXEL_FORMAT_BGR_565:
    case HAL_PIXEL_FORMAT_RGBA_5551:
    case HAL_PIXEL_FORMAT_RGBA_4444:
      stride[0] = static_cast<uint32_t>(hnd->width * 2);
      break;
    case HAL_PIXEL_FORMAT_RGB_888:
      stride[0] = static_cast<uint32_t>(hnd->width * 3);
      break;
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_BGRX_8888:
    case HAL_PIXEL_FORMAT_RGBA_1010102:
    case HAL_PIXEL_FORMAT_ARGB_2101010:
    case HAL_PIXEL_FORMAT_RGBX_1010102:
    case HAL_PIXEL_FORMAT_XRGB_2101010:
    case HAL_PIXEL_FORMAT_BGRA_1010102:
    case HAL_PIXEL_FORMAT_ABGR_2101010:
    case HAL_PIXEL_FORMAT_BGRX_1010102:
    case HAL_PIXEL_FORMAT_XBGR_2101010:
      stride[0] = static_cast<uint32_t>(hnd->width * 4);
      break;
  }

  // Format is RGB
  if (stride[0]) {
    return 0;
  }

  (*num_planes)++;
  int ret = GetYUVPlaneInfo(hnd, yuvPlaneInfo);
  if (ret < 0) {
    ALOGE("%s failed", __FUNCTION__);
    return ret;
  }

  // We are only returning buffer layout for progressive or single field formats.
  struct android_ycbcr yuvInfo = yuvPlaneInfo[0];
  stride[0] = static_cast<uint32_t>(yuvInfo.ystride);
  offset[0] = static_cast<uint32_t>(reinterpret_cast<uint64_t>(yuvInfo.y) - hnd->base);
  stride[1] = static_cast<uint32_t>(yuvInfo.cstride);
  switch (hnd->format) {
    case HAL_PIXEL_FORMAT_YCbCr_420_SP:
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010:
    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
      offset[1] = static_cast<uint32_t>(reinterpret_cast<uint64_t>(yuvInfo.cb) - hnd->base);
      break;
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
    case HAL_PIXEL_FORMAT_YCrCb_422_SP:
      offset[1] = static_cast<uint32_t>(reinterpret_cast<uint64_t>(yuvInfo.cr) - hnd->base);
      break;
    case HAL_PIXEL_FORMAT_YV12:
      offset[1] = static_cast<uint32_t>(reinterpret_cast<uint64_t>(yuvInfo.cr) - hnd->base);
      stride[2] = static_cast<uint32_t>(yuvInfo.cstride);
      offset[2] = static_cast<uint32_t>(reinterpret_cast<uint64_t>(yuvInfo.cb) - hnd->base);
      (*num_planes)++;
      break;
    default:
      ALOGW("%s: Unsupported format", __FUNCTION__);
      ret = -EINVAL;
  }

  if (hnd->flags & private_handle_t::PRIV_FLAGS_UBWC_ALIGNED) {
    std::fill(offset, offset + 4, 0);
  }

  return 0;
}

void GetGpuResourceSizeAndDimensions(const BufferInfo &info, unsigned int *size,
                                     unsigned int *alignedw, unsigned int *alignedh,
                                     GraphicsMetadata *graphics_metadata) {
  GetAlignedWidthAndHeight(info, alignedw, alignedh);
  AdrenoMemInfo* adreno_mem_info = AdrenoMemInfo::GetInstance();
  graphics_metadata->size = adreno_mem_info->AdrenoGetMetadataBlobSize();
  uint64_t adreno_usage = info.usage;
  // If gralloc disables UBWC based on any of the checks,
  // we pass modified usage flag to adreno to convey this.
  int is_ubwc_enabled = IsUBwcEnabled(info.format, info.usage);
  if (!is_ubwc_enabled) {
    adreno_usage &= ~(GRALLOC_USAGE_PRIVATE_ALLOC_UBWC);
  } else {
    adreno_usage |= GRALLOC_USAGE_PRIVATE_ALLOC_UBWC;
  }

  // Call adreno api for populating metadata blob
  // Layer count is for 2D/Cubemap arrays and depth is used for 3D slice
  // Using depth to pass layer_count here
  int ret = adreno_mem_info->AdrenoInitMemoryLayout(graphics_metadata->data, info.width,
                                                    info.height, info.layer_count, /* depth */
                                                    info.format, 1, is_ubwc_enabled,
                                                    adreno_usage, 1);
  if (ret != 0) {
    ALOGE("%s Graphics metadata init failed", __FUNCTION__);
    *size = 0;
    return;
  }
  // Call adreno api with the metadata blob to get buffer size
  *size = adreno_mem_info->AdrenoGetAlignedGpuBufferSize(graphics_metadata->data);
}

bool CanUseAdrenoForSize(int buffer_type, uint64_t usage) {
  if (buffer_type == BUFFER_TYPE_VIDEO || !GetAdrenoSizeAPIStatus()) {
    return false;
  }

  if ((usage & BufferUsage::PROTECTED) && ((usage & BufferUsage::CAMERA_OUTPUT) ||
      (usage & GRALLOC_USAGE_PRIVATE_SECURE_DISPLAY))) {
    return false;
  }

  return true;
}

bool GetAdrenoSizeAPIStatus() {
  AdrenoMemInfo* adreno_mem_info = AdrenoMemInfo::GetInstance();
  if (adreno_mem_info) {
    return adreno_mem_info->AdrenoSizeAPIAvaliable();
  }
  return false;
}

int GetBufferType(int inputFormat) {
  return IsYuvFormat(inputFormat) ? BUFFER_TYPE_VIDEO : BUFFER_TYPE_UI;
}

int GetYUVPlaneInfo(const BufferInfo &info, int32_t format, int32_t width, int32_t height,
                    int32_t flags, int *plane_count, PlaneLayoutInfo *plane_info) {
  int err = 0;
  unsigned int y_stride, c_stride, y_height, c_height, y_size, c_size;
  uint64_t yOffset, cOffset, crOffset, cbOffset;
  int h_subsampling = 0, v_subsampling = 0;
  switch (format) {
    // Semiplanar
    case HAL_PIXEL_FORMAT_YCbCr_420_SP:
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
    case HAL_PIXEL_FORMAT_NV12_HEIF:  // Same as YCbCr_420_SP_VENUS
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_YCrCb_422_SP:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_ADRENO:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_NV21_ZSL:
    case HAL_PIXEL_FORMAT_RAW16:
    case HAL_PIXEL_FORMAT_Y16:
    case HAL_PIXEL_FORMAT_RAW10:
    case HAL_PIXEL_FORMAT_RAW8:
    case HAL_PIXEL_FORMAT_Y8:
      *plane_count = 2;
      GetYuvSPPlaneInfo(info, format, width, height, 1, plane_info);
      GetYuvSubSamplingFactor(format, &h_subsampling, &v_subsampling);
      plane_info[0].h_subsampling = 0;
      plane_info[0].v_subsampling = 0;
      plane_info[1].h_subsampling = h_subsampling;
      plane_info[1].v_subsampling = v_subsampling;
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
      GetYuvSubSamplingFactor(format, &h_subsampling, &v_subsampling);
      if (flags & LAYOUT_INTERLACED_FLAG) {
        *plane_count = 8;
        GetYuvUbwcInterlacedSPPlaneInfo(width, height, plane_info);
        plane_info[0].step = plane_info[4].step = 1;
        plane_info[1].step = plane_info[5].step = 2;
        plane_info[0].h_subsampling = plane_info[4].h_subsampling = 0;
        plane_info[0].v_subsampling = plane_info[4].v_subsampling = 0;
        plane_info[1].h_subsampling = plane_info[5].h_subsampling = h_subsampling;
        plane_info[1].v_subsampling = plane_info[5].v_subsampling = v_subsampling;
        plane_info[2].h_subsampling = plane_info[3].h_subsampling = 0;
        plane_info[2].v_subsampling = plane_info[3].v_subsampling = 0;
        plane_info[2].step = plane_info[3].step = 0;
        plane_info[6].h_subsampling = plane_info[7].h_subsampling = 0;
        plane_info[6].v_subsampling = plane_info[7].v_subsampling = 0;
        plane_info[6].step = plane_info[7].step = 0;
      } else {
        *plane_count = 4;
        GetYuvUbwcSPPlaneInfo(width, height, COLOR_FMT_NV12_UBWC, plane_info);
        plane_info[0].h_subsampling = 0;
        plane_info[0].v_subsampling = 0;
        plane_info[0].step = 1;
        plane_info[1].h_subsampling = h_subsampling;
        plane_info[1].v_subsampling = v_subsampling;
        plane_info[1].step = 2;
        plane_info[2].h_subsampling = plane_info[3].h_subsampling = 0;
        plane_info[2].v_subsampling = plane_info[3].v_subsampling = 0;
        plane_info[2].step = plane_info[3].step = 0;
      }
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_P010:
      *plane_count = 2;
      GetYuvSPPlaneInfo(info, format, width, height, 2, plane_info);
      GetYuvSubSamplingFactor(format, &h_subsampling, &v_subsampling);
      plane_info[0].h_subsampling = 0;
      plane_info[0].v_subsampling = 0;
      plane_info[1].h_subsampling = h_subsampling;
      plane_info[1].v_subsampling = v_subsampling;
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
      *plane_count = 4;
      GetYuvUbwcSPPlaneInfo(width, height, COLOR_FMT_NV12_BPP10_UBWC, plane_info);
      GetYuvSubSamplingFactor(format, &h_subsampling, &v_subsampling);
      plane_info[0].h_subsampling = 0;
      plane_info[0].v_subsampling = 0;
      plane_info[1].step = 1;
      plane_info[1].h_subsampling = h_subsampling;
      plane_info[1].v_subsampling = v_subsampling;
      plane_info[1].step = 3;
      plane_info[2].h_subsampling = plane_info[3].h_subsampling = 0;
      plane_info[2].v_subsampling = plane_info[3].v_subsampling = 0;
      plane_info[2].step = plane_info[3].step = 0;
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
      *plane_count = 4;
      GetYuvUbwcSPPlaneInfo(width, height, COLOR_FMT_P010_UBWC, plane_info);
      GetYuvSubSamplingFactor(format, &h_subsampling, &v_subsampling);
      plane_info[0].h_subsampling = 0;
      plane_info[0].v_subsampling = 0;
      plane_info[1].step = 1;
      plane_info[1].h_subsampling = h_subsampling;
      plane_info[1].v_subsampling = v_subsampling;
      plane_info[1].step = 4;
      plane_info[2].h_subsampling = plane_info[3].h_subsampling = 0;
      plane_info[2].v_subsampling = plane_info[3].v_subsampling = 0;
      plane_info[2].step = plane_info[3].step = 0;
      break;

    case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
      *plane_count = 2;
      y_stride = VENUS_Y_STRIDE(COLOR_FMT_P010, width);
      c_stride = VENUS_UV_STRIDE(COLOR_FMT_P010, width);
      y_height = VENUS_Y_SCANLINES(COLOR_FMT_P010, height);
      y_size = y_stride * y_height;
      yOffset = 0;
      cOffset = y_size;
      c_height = VENUS_UV_SCANLINES(COLOR_FMT_P010, INT(height));
      c_size = c_stride * c_height;
      GetYuvSubSamplingFactor(format, &h_subsampling, &v_subsampling);

      plane_info[0].component = (PlaneComponent)PLANE_COMPONENT_Y;
      plane_info[0].offset = (uint32_t)yOffset;
      plane_info[0].stride = static_cast<int32_t>(UINT(width));
      plane_info[0].stride_bytes = static_cast<int32_t>(y_stride);
      plane_info[0].scanlines = static_cast<int32_t>(y_height);
      plane_info[0].size = static_cast<uint32_t>(y_size);
      plane_info[0].step = 1;
      plane_info[0].h_subsampling = 0;
      plane_info[0].v_subsampling = 0;

      plane_info[1].component = (PlaneComponent)(PLANE_COMPONENT_Cb | PLANE_COMPONENT_Cr);
      plane_info[1].offset = (uint32_t)cOffset;
      plane_info[1].stride = static_cast<int32_t>(UINT(width));
      plane_info[1].stride_bytes = static_cast<int32_t>(c_stride);
      plane_info[1].scanlines = static_cast<int32_t>(c_height);
      plane_info[1].size = static_cast<uint32_t>(c_size);
      plane_info[1].step = 4;
      plane_info[1].h_subsampling = h_subsampling;
      plane_info[1].v_subsampling = v_subsampling;
      break;

      // Planar
    case HAL_PIXEL_FORMAT_YV12:
      if ((info.width & 1) || (info.height & 1)) {
        ALOGE("w or h is odd for the YV12 format");
        err = -EINVAL;
        return err;
      }
      *plane_count = 3;
      y_stride = width;
      c_stride = ALIGN(width / 2, 16);
      y_height = UINT(height);
      y_size = (y_stride * y_height);
      height = height >> 1;
      c_height = UINT(height);
      c_size = (c_stride * c_height);
      yOffset = 0;
      crOffset = y_size;
      cbOffset = (y_size + c_size);
      GetYuvSubSamplingFactor(format, &h_subsampling, &v_subsampling);

      plane_info[0].component = (PlaneComponent)PLANE_COMPONENT_Y;
      plane_info[0].offset = (uint32_t)yOffset;
      plane_info[0].stride = static_cast<int32_t>(UINT(width));
      plane_info[0].stride_bytes = static_cast<int32_t>(y_stride);
      plane_info[0].scanlines = static_cast<int32_t>(y_height);
      plane_info[0].size = static_cast<uint32_t>(y_size);
      plane_info[0].step = 1;
      plane_info[0].h_subsampling = 0;
      plane_info[0].v_subsampling = 0;

      plane_info[1].component = (PlaneComponent)PLANE_COMPONENT_Cb;
      plane_info[1].offset = (uint32_t)cbOffset;
      plane_info[2].component = (PlaneComponent)PLANE_COMPONENT_Cr;
      plane_info[2].offset = (uint32_t)crOffset;
      for (int i = 1; i < 3; i++) {
        plane_info[i].stride = static_cast<int32_t>(UINT(width));
        plane_info[i].stride_bytes = static_cast<int32_t>(c_stride);
        plane_info[i].scanlines = static_cast<int32_t>(c_height);
        plane_info[i].size = static_cast<uint32_t>(c_size);
        plane_info[i].step = 1;
        plane_info[i].h_subsampling = h_subsampling;
        plane_info[i].v_subsampling = v_subsampling;
      }
      break;
    case HAL_PIXEL_FORMAT_CbYCrY_422_I:
      if (info.width & 1) {
        ALOGE("width is odd for the YUV422_SP format");
        err = -EINVAL;
        return err;
      }
      *plane_count = 1;
      y_stride = width * 2;
      y_height = UINT(height);
      y_size = y_stride * y_height;
      yOffset = 0;
      plane_info[0].component = (PlaneComponent)PLANE_COMPONENT_Y;
      plane_info[0].offset = (uint32_t)yOffset;
      plane_info[0].stride = static_cast<int32_t>(UINT(width));
      plane_info[0].stride_bytes = static_cast<int32_t>(y_stride);
      plane_info[0].scanlines = static_cast<int32_t>(y_height);
      plane_info[0].size = static_cast<uint32_t>(y_size);
      plane_info[0].step = 1;
      plane_info[0].h_subsampling = 0;
      plane_info[0].v_subsampling = 0;
      break;

      // Unsupported formats
    case HAL_PIXEL_FORMAT_YCbCr_422_I:
    case HAL_PIXEL_FORMAT_YCrCb_422_I:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED:
    default:
      *plane_count = 0;
      ALOGD("%s: Invalid format passed: 0x%x", __FUNCTION__, format);
      err = -EINVAL;
  }
  return err;
}

void GetYuvSubSamplingFactor(int32_t format, int *h_subsampling, int *v_subsampling) {
  switch (format) {
    case HAL_PIXEL_FORMAT_YCbCr_420_SP:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010:
    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_ADRENO:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_NV12_ENCODEABLE:  // Same as YCbCr_420_SP_VENUS
    case HAL_PIXEL_FORMAT_NV21_ENCODEABLE:
    case HAL_PIXEL_FORMAT_NV21_ZSL:
    case HAL_PIXEL_FORMAT_YV12:
      *h_subsampling = 1;
      *v_subsampling = 1;
      break;
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
    case HAL_PIXEL_FORMAT_YCrCb_422_SP:
    case HAL_PIXEL_FORMAT_CbYCrY_422_I:
      *h_subsampling = 1;
      *v_subsampling = 0;
      break;
    case HAL_PIXEL_FORMAT_RAW16:
    case HAL_PIXEL_FORMAT_Y16:
    case HAL_PIXEL_FORMAT_RAW12:
    case HAL_PIXEL_FORMAT_RAW10:
    case HAL_PIXEL_FORMAT_Y8:
    case HAL_PIXEL_FORMAT_BLOB:
    case HAL_PIXEL_FORMAT_RAW_OPAQUE:
    case HAL_PIXEL_FORMAT_NV12_HEIF:
    default:
      *h_subsampling = 0;
      *v_subsampling = 0;
      break;
  }
}

void CopyPlaneLayoutInfotoAndroidYcbcr(uint64_t base, int plane_count, PlaneLayoutInfo *plane_info,
                                       struct android_ycbcr *ycbcr) {
  ycbcr->y = reinterpret_cast<void *>(base + plane_info[0].offset);
  ycbcr->ystride = plane_info[0].stride_bytes;
  if (plane_count == 1) {
    ycbcr->cb = NULL;
    ycbcr->cr = NULL;
    ycbcr->cstride = 0;
    ycbcr->chroma_step = 0;
  } else if (plane_count == 2 || plane_count == 4 || plane_count == 8) {
    /* For YUV semiplanar :-
     *   - In progressive & linear case plane count is 2 and plane_info[0] will
     *     contain info about Y plane and plane_info[1] will contain info about UV plane.
     *   - In progressive & compressed case plane count is 4 then plane_info[0] will
     *     contain info about Y plane and plane_info[1] will contain info about UV plane.
     *     Remaining two plane (plane_info[2] & plane_info[3]) contain info about the
     *     Y_Meta_Plane and UV_Meta_Plane.
     *   - In interlaced & compressed case plane count is 8 then plane_info[0], plane_info[1],
     *     plane_info[4] & plane_info[5] will contain info about Y_plane, UV_plane, Y_plane
     *     & UV_plane. Remaining plane will contain info about the meta planes. As in this case
     *     this API is called twice through GetYUVPlaneInfo() with address of plane_info[0] &
     *     plane_info[4], so this will calculate the information accordingly and will fill the
     *     ycbcr structure with interlaced plane info only.
     */
    ycbcr->cb = reinterpret_cast<void *>(base + plane_info[1].offset);
    ycbcr->cr = reinterpret_cast<void *>(base + plane_info[1].offset + 1);
    ycbcr->cstride = plane_info[1].stride_bytes;
    ycbcr->chroma_step = plane_info[1].step;
  } else if (plane_count == 3) {
    /* For YUV planar :-
     * Plane size is 3 and plane_info[0], plane_info[1], plane_info[2] will
     * contain info about y_plane, cb_plane and cr_plane accordingly.
     */
    ycbcr->cb = reinterpret_cast<void *>(base + plane_info[1].offset);
    ycbcr->cr = reinterpret_cast<void *>(base + plane_info[2].offset);
    ycbcr->cstride = plane_info[1].stride_bytes;
    ycbcr->chroma_step = plane_info[1].step;
  }
}

bool HasAlphaComponent(int32_t format) {
  switch (format) {
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_RGBA_5551:
    case HAL_PIXEL_FORMAT_RGBA_4444:
    case HAL_PIXEL_FORMAT_RGBA_1010102:
    case HAL_PIXEL_FORMAT_ARGB_2101010:
    case HAL_PIXEL_FORMAT_BGRA_1010102:
    case HAL_PIXEL_FORMAT_ABGR_2101010:
    case HAL_PIXEL_FORMAT_RGBA_FP16:
      return true;
    default:
      return false;
  }
}

void GetRGBPlaneInfo(const BufferInfo &info, int32_t format, int32_t width, int32_t height,
                     int32_t /* flags */, int *plane_count, PlaneLayoutInfo *plane_info) {
  uint64_t usage = info.usage;
  *plane_count = 1;
  uint32_t bpp = 0;
  if (IsUncompressedRGBFormat(format)) {
    bpp = GetBppForUncompressedRGB(format);
  }
  plane_info->component =
      (PlaneComponent)(PLANE_COMPONENT_R | PLANE_COMPONENT_G | PLANE_COMPONENT_B);
  if (HasAlphaComponent(format)) {
    plane_info->component = (PlaneComponent)(plane_info->component | PLANE_COMPONENT_A);
  }
  plane_info->size = GetSize(info, width, height);
  plane_info->step = bpp;
  plane_info->offset = GetRgbMetaSize(format, width, height, usage);
  plane_info->h_subsampling = 0;
  plane_info->v_subsampling = 0;
  plane_info->stride = width;
  plane_info->stride_bytes = width * plane_info->step;
  plane_info->scanlines = height;
}

// TODO(tbalacha): tile vs ubwc -- may need to find a diff way to differentiate
void GetDRMFormat(uint32_t format, uint32_t flags, uint32_t *drm_format,
                  uint64_t *drm_format_modifier) {
  bool compressed = (flags & private_handle_t::PRIV_FLAGS_UBWC_ALIGNED) ? true : false;
  switch (format) {
    case HAL_PIXEL_FORMAT_RGBA_8888:
      *drm_format = DRM_FORMAT_ABGR8888;
      break;
    case HAL_PIXEL_FORMAT_RGBA_5551:
      *drm_format = DRM_FORMAT_ABGR1555;
      break;
    case HAL_PIXEL_FORMAT_RGBA_4444:
      *drm_format = DRM_FORMAT_ABGR4444;
      break;
    case HAL_PIXEL_FORMAT_BGRA_8888:
      *drm_format = DRM_FORMAT_ARGB8888;
      break;
    case HAL_PIXEL_FORMAT_RGBX_8888:
      *drm_format = DRM_FORMAT_XBGR8888;
      if (compressed)
        *drm_format_modifier = DRM_FORMAT_MOD_QCOM_COMPRESSED;
      break;
    case HAL_PIXEL_FORMAT_BGRX_8888:
      *drm_format = DRM_FORMAT_XRGB8888;
      break;
    case HAL_PIXEL_FORMAT_RGB_888:
      *drm_format = DRM_FORMAT_BGR888;
      break;
    case HAL_PIXEL_FORMAT_RGB_565:
      *drm_format = DRM_FORMAT_BGR565;
      break;
    case HAL_PIXEL_FORMAT_BGR_565:
      *drm_format = DRM_FORMAT_BGR565;
      if (compressed)
        *drm_format_modifier = DRM_FORMAT_MOD_QCOM_COMPRESSED;
      break;
    case HAL_PIXEL_FORMAT_RGBA_1010102:
      *drm_format = DRM_FORMAT_ABGR2101010;
      if (compressed)
        *drm_format_modifier = DRM_FORMAT_MOD_QCOM_COMPRESSED;
      break;
    case HAL_PIXEL_FORMAT_ARGB_2101010:
      *drm_format = DRM_FORMAT_BGRA1010102;
      break;
    case HAL_PIXEL_FORMAT_RGBX_1010102:
      *drm_format = DRM_FORMAT_XBGR2101010;
      if (compressed)
        *drm_format_modifier = DRM_FORMAT_MOD_QCOM_COMPRESSED;
      break;
    case HAL_PIXEL_FORMAT_XRGB_2101010:
      *drm_format = DRM_FORMAT_BGRX1010102;
      break;
    case HAL_PIXEL_FORMAT_BGRA_1010102:
      *drm_format = DRM_FORMAT_ARGB2101010;
      break;
    case HAL_PIXEL_FORMAT_ABGR_2101010:
      *drm_format = DRM_FORMAT_RGBA1010102;
      break;
    case HAL_PIXEL_FORMAT_BGRX_1010102:
      *drm_format = DRM_FORMAT_XRGB2101010;
      break;
    case HAL_PIXEL_FORMAT_XBGR_2101010:
      *drm_format = DRM_FORMAT_RGBX1010102;
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS:
      *drm_format = DRM_FORMAT_NV12;
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS_UBWC:
      *drm_format = DRM_FORMAT_NV12;
      if (compressed) {
        *drm_format_modifier = DRM_FORMAT_MOD_QCOM_COMPRESSED;
      } else {
        *drm_format_modifier = DRM_FORMAT_MOD_QCOM_TILE;
      }
      break;
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
      *drm_format = DRM_FORMAT_NV21;
      break;
    case HAL_PIXEL_FORMAT_YCrCb_420_SP_VENUS:
      *drm_format = DRM_FORMAT_NV21;
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_P010:
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_VENUS:
      *drm_format = DRM_FORMAT_NV12;
      *drm_format_modifier = DRM_FORMAT_MOD_QCOM_DX;
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_P010_UBWC:
      *drm_format = DRM_FORMAT_NV12;
      if (compressed) {
        *drm_format_modifier = DRM_FORMAT_MOD_QCOM_COMPRESSED | DRM_FORMAT_MOD_QCOM_DX;
      } else {
        *drm_format_modifier = DRM_FORMAT_MOD_QCOM_TILE | DRM_FORMAT_MOD_QCOM_DX;
      }
      break;
    case HAL_PIXEL_FORMAT_YCbCr_420_TP10_UBWC:
      *drm_format = DRM_FORMAT_NV12;
      if (compressed) {
        *drm_format_modifier =
            DRM_FORMAT_MOD_QCOM_COMPRESSED | DRM_FORMAT_MOD_QCOM_DX | DRM_FORMAT_MOD_QCOM_TIGHT;
      } else {
        *drm_format_modifier =
            DRM_FORMAT_MOD_QCOM_TILE | DRM_FORMAT_MOD_QCOM_DX | DRM_FORMAT_MOD_QCOM_TIGHT;
      }
      break;
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
      *drm_format = DRM_FORMAT_NV16;
      break;
      /*
    TODO: No HAL_PIXEL_FORMAT equivalent?
    case kFormatYCrCb422H2V1SemiPlanar:
      *drm_format = DRM_FORMAT_NV61;
      break;*/
    case HAL_PIXEL_FORMAT_YV12:
      *drm_format = DRM_FORMAT_YVU420;
      break;
    default:
      ALOGE("Unsupported format %d", format);
  }
}

}  // namespace gralloc
