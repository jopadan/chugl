/*----------------------------------------------------------------------------
 ChuGL: Unified Audiovisual Programming in ChucK

 Copyright (c) 2023 Andrew Zhu Aday and Ge Wang. All rights reserved.
   http://chuck.stanford.edu/chugl/
   http://chuck.cs.princeton.edu/chugl/

 MIT License

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
-----------------------------------------------------------------------------*/
#include <chuck/chugin.h>

#include "sg_command.h"
#include "sg_component.h"

#include "ulib_helper.h"

#include "core/log.h"

#include <stb/stb_image.h>

#if 0
enum WGPUTextureUsage {
    WGPUTextureUsage_CopySrc = 0x00000001,
    WGPUTextureUsage_CopyDst = 0x00000002,
    WGPUTextureUsage_TextureBinding = 0x00000004,
    WGPUTextureUsage_StorageBinding = 0x00000008,
    WGPUTextureUsage_RenderAttachment = 0x00000010,
}

enum WGPUTextureDimension {
    WGPUTextureDimension_1D = 0x00000000,
    WGPUTextureDimension_2D = 0x00000001,
    WGPUTextureDimension_3D = 0x00000002,
}

struct WGPUExtent3D {
    uint32_t width;
    uint32_t height;
    uint32_t depthOrArrayLayers;
}

struct WGPUTextureFormat {
    WGPUTextureFormat_RGBA8Unorm = 0x00000012,
    WGPUTextureFormat_RGBA16Float = 0x00000021,
    WGPUTextureFormat_Depth24PlusStencil8 = 0x00000028,
}

struct WGPUTextureDescriptor {
    WGPUTextureUsageFlags usage;    // 
    WGPUTextureDimension dimension; //  
    WGPUExtent3D size;
    WGPUTextureFormat format;
    // uint32_t mipLevelCount; // always gen mips
    // uint32_t sampleCount;   // don't expose for now
}

typedef struct WGPUOrigin3D {
    uint32_t x;
    uint32_t y;
    uint32_t z;
}

enum WGPUTextureAspect {
    WGPUTextureAspect_All = 0x00000000,
    WGPUTextureAspect_StencilOnly = 0x00000001,
    WGPUTextureAspect_DepthOnly = 0x00000002,
    WGPUTextureAspect_Force32 = 0x7FFFFFFF
}

struct WGPUImageCopyTexture {
    WGPUTexture texture;
    uint32_t mipLevel;
    WGPUOrigin3D origin;
    WGPUTextureAspect aspect; // default to Aspect_All for non-depth textures
}

struct WGPUTextureDataLayout {
    uint64_t offset; // offset into CPU-pointer void* data
    uint32_t bytesPerRow; // minimum value for bytesPerRow that is set to 256 by the API.
    uint32_t rowsPerImage; // required if there are multiple images (3D or 2D array textures)
}

void wgpuQueueWriteTexture(
    WGPUQueue queue, 
    WGPUImageCopyTexture* destination, 
    void* data, 
    size_t dataSize, 
    WGPUTextureDataLayout* dataLayout,  // layout of cpu-side void* data
    WGPUExtent3D* writeSize             // size of destination region in texture
);
#endif

void ulib_texture_createDefaults(CK_DL_API API);

// TextureSampler ---------------------------------------------------------------------
CK_DLL_CTOR(sampler_ctor);

// TextureDesc -----------------------------------------------------------------

static t_CKUINT texture_desc_format_offset    = 0;
static t_CKUINT texture_desc_dimension_offset = 0;
static t_CKUINT texture_desc_width_offset     = 0;
static t_CKUINT texture_desc_height_offset    = 0;
static t_CKUINT texture_desc_depth_offset     = 0;
static t_CKUINT texture_desc_usage_offset     = 0;
// static t_CKUINT texture_desc_samples_offset   = 0; // not exposing for now
static t_CKUINT texture_desc_mips_offset = 0; // not exposing for now
CK_DLL_CTOR(texture_desc_ctor);

// TextureWriteDesc -----------------------------------------------------------------
CK_DLL_CTOR(texture_write_desc_ctor);
// dst image location
static t_CKUINT texture_write_desc_mip_offset      = 0;
static t_CKUINT texture_write_desc_offset_x_offset = 0;
static t_CKUINT texture_write_desc_offset_y_offset = 0;
static t_CKUINT texture_write_desc_offset_z_offset = 0;
// dst region size
static t_CKUINT texture_write_desc_width_offset  = 0;
static t_CKUINT texture_write_desc_height_offset = 0;
static t_CKUINT texture_write_desc_depth_offset  = 0;

// TextureWriteDesc -----------------------------------------------------------------
CK_DLL_CTOR(texture_load_desc_ctor);

static t_CKUINT texture_load_desc_flip_y_offset   = 0;
static t_CKUINT texture_load_desc_gen_mips_offset = 0;

// Texture ---------------------------------------------------------------------
CK_DLL_CTOR(texture_ctor);
CK_DLL_CTOR(texture_ctor_with_desc);

CK_DLL_MFUN(texture_get_format);
CK_DLL_MFUN(texture_get_dimension);
CK_DLL_MFUN(texture_get_width);
CK_DLL_MFUN(texture_get_height);
CK_DLL_MFUN(texture_get_depth);
CK_DLL_MFUN(texture_get_usage);
CK_DLL_MFUN(texture_get_mips);

CK_DLL_MFUN(texture_write);
CK_DLL_MFUN(texture_write_with_desc);

// loading images
CK_DLL_SFUN(texture_load_2d_file);
CK_DLL_SFUN(texture_load_2d_file_with_params);
CK_DLL_SFUN(texture_load_cubemap);

static void ulib_texture_query(Chuck_DL_Query* QUERY)
{
    { // Sampler (only passed by value)
        QUERY->begin_class(QUERY, "TextureSampler", "Object");
        DOC_CLASS("Texture Sampler -- options for sampling a texture");

        // static vars
        static t_CKINT WRAP_REPEAT    = SG_SAMPLER_WRAP_REPEAT;
        static t_CKINT WRAP_MIRROR    = SG_SAMPLER_WRAP_MIRROR_REPEAT;
        static t_CKINT WRAP_CLAMP     = SG_SAMPLER_WRAP_CLAMP_TO_EDGE;
        static t_CKINT FILTER_NEAREST = SG_SAMPLER_FILTER_NEAREST;
        static t_CKINT FILTER_LINEAR  = SG_SAMPLER_FILTER_LINEAR;
        QUERY->add_svar(QUERY, "int", "Wrap_Repeat", true, &WRAP_REPEAT);
        QUERY->add_svar(QUERY, "int", "Wrap_Mirror", true, &WRAP_MIRROR);
        QUERY->add_svar(QUERY, "int", "Wrap_Clamp", true, &WRAP_CLAMP);
        QUERY->add_svar(QUERY, "int", "Filter_Nearest", true, &FILTER_NEAREST);
        QUERY->add_svar(QUERY, "int", "Filter_Linear", true, &FILTER_LINEAR);

        // member vars
        sampler_offset_wrapU = QUERY->add_mvar(QUERY, "int", "wrapU", false);
        DOC_VAR(
          "U-axis (horizontal) wrap mode. Valid values are TextureSampler.Wrap_Repeat, "
          "TextureSampler.Wrap_Mirror, TextureSampler.Wrap_Clamp");
        sampler_offset_wrapV = QUERY->add_mvar(QUERY, "int", "wrapV", false);
        DOC_VAR(
          "V-axis (vertical) wrap mode. Valid values are TextureSampler.Wrap_Repeat, "
          "TextureSampler.Wrap_Mirror, TextureSampler.Wrap_Clamp");
        sampler_offset_wrapW = QUERY->add_mvar(QUERY, "int", "wrapW", false);
        DOC_VAR(
          "W-axis wrap mode. Valid values are TextureSampler.Wrap_Repeat, "
          "TextureSampler.Wrap_Mirror, TextureSampler.Wrap_Clamp");
        sampler_offset_filterMin = QUERY->add_mvar(QUERY, "int", "filterMin", false);
        DOC_VAR(
          "Minification filter. Valid values are TextureSampler.Filter_Nearest, "
          "TextureSampler.Filter_Linear");
        sampler_offset_filterMag = QUERY->add_mvar(QUERY, "int", "filterMag", false);
        DOC_VAR(
          "Magnification filter. Valid values are TextureSampler.Filter_Nearest, "
          "TextureSampler.Filter_Linear");
        sampler_offset_filterMip = QUERY->add_mvar(QUERY, "int", "filterMip", false);
        DOC_VAR(
          "Mip level filter. Valid values are TextureSampler.Filter_Nearest, "
          "TextureSampler.Filter_Linear");

        // constructor
        QUERY->add_ctor(QUERY, sampler_ctor); // default constructor

        QUERY->end_class(QUERY); // Sampler
    }

    { // TextureDesc
        BEGIN_CLASS("TextureDesc", "Object");
        DOC_CLASS("Texture Descriptor -- options for creating a texture");

        CTOR(texture_desc_ctor);

        // member vars
        texture_desc_format_offset = MVAR("int", "format", false);
        DOC_VAR(
          "Texture format. Valid options are defined in the Texture.Format_* enum. "
          "Default is Texture.Format_RGBA8Unorm");

        texture_desc_dimension_offset = MVAR("int", "dimension", false);
        DOC_VAR(
          "Texture dimension. Valid options are defined in the Texture.Dimension_* "
          "enum. Default is Texture.Dimension_2D");

        texture_desc_width_offset = MVAR("int", "width", false);
        DOC_VAR("Width in texels. Default is 1");

        texture_desc_height_offset = MVAR("int", "height", false);
        DOC_VAR("Height in texels. Default is 1");

        texture_desc_depth_offset = MVAR("int", "depth", false);
        DOC_VAR("Depth in texels. Default is 1");

        texture_desc_usage_offset = MVAR("int", "usage", false);
        DOC_VAR(
          "Bit mask of texture usage flags. Valid flags are defined in the "
          "Texture.Usage_* enum. Default is Texture.Usage_All, which enables all "
          "usages");
        // texture_desc_samples_offset   = MVAR("int", "samples");
        texture_desc_mips_offset = MVAR("int", "mips", false);
        DOC_VAR("Number of mip levels. Default is 1");

        END_CLASS();
    } // end TextureDesc

    { // TextureWriteDesc
        BEGIN_CLASS("TextureWriteDesc", "Object");
        DOC_CLASS("Options for writing to a texture");

        CTOR(texture_write_desc_ctor);

        texture_write_desc_mip_offset = MVAR("int", "mip", false);
        DOC_VAR("Which mip level to write to. Default is 0 (base level)");

        texture_write_desc_offset_x_offset = MVAR("int", "x", false);
        DOC_VAR("X offset of write region. Default 0");

        texture_write_desc_offset_y_offset = MVAR("int", "y", false);
        DOC_VAR("Y offset of write region. Default 0");

        texture_write_desc_offset_z_offset = MVAR("int", "z", false);
        DOC_VAR("Z offset of write region. Default 0");

        texture_write_desc_width_offset = MVAR("int", "width", false);
        DOC_VAR("Width of write region. Default 0");

        texture_write_desc_height_offset = MVAR("int", "height", false);
        DOC_VAR("Height of write region. Default 0");

        texture_write_desc_depth_offset = MVAR("int", "depth", false);
        DOC_VAR("Depth of write region. Default 0");

        END_CLASS();
    };

    { // TextureLoadDesc
        BEGIN_CLASS("TextureLoadDesc", "Object");
        DOC_CLASS("Options for loading a texture from a file");

        CTOR(texture_load_desc_ctor);

        texture_load_desc_flip_y_offset = MVAR("int", "flip_y", false);
        DOC_VAR("Flip the image vertically before loading. Default false");

        texture_load_desc_gen_mips_offset = MVAR("int", "gen_mips", false);
        DOC_VAR("Generate mipmaps for the texture. Default true");

        END_CLASS();
    }

    // Texture
    {
        BEGIN_CLASS(SG_CKNames[SG_COMPONENT_TEXTURE], SG_CKNames[SG_COMPONENT_BASE]);
        DOC_CLASS(
          "Texture class. Immutable, meaning properties (e.g. format, dimension, size, "
          "mip levels) "
          "cannot be changed after creation. You can, however, write data to the "
          "texture");
        ADD_EX("deep/game_of_life.ck");
        ADD_EX("deep/audio_donut.ck");
        ADD_EX("deep/snowstorm.ck");
        ADD_EX("basic/skybox.ck");

        // svars ---------------
        static t_CKINT texture_usage_copy_src        = WGPUTextureUsage_CopySrc;
        static t_CKINT texture_usage_copy_dst        = WGPUTextureUsage_CopyDst;
        static t_CKINT texture_usage_texture_binding = WGPUTextureUsage_TextureBinding;
        static t_CKINT texture_usage_storage_binding = WGPUTextureUsage_StorageBinding;
        static t_CKINT texture_usage_render_attachment
          = WGPUTextureUsage_RenderAttachment;
        static t_CKINT texture_usage_all = WGPUTextureUsage_All;
        SVAR("int", "Usage_CopySrc", &texture_usage_copy_src);
        DOC_VAR(
          "Texture usage flag: can be used as a source for copy/write operations");
        SVAR("int", "Usage_CopyDst", &texture_usage_copy_dst);
        DOC_VAR(
          "Texture usage flag: can be used destination for copy/write operations");
        SVAR("int", "Usage_TextureBinding", &texture_usage_texture_binding);
        DOC_VAR("Texture usage flag: texture can be bound to a shader");
        SVAR("int", "Usage_StorageBinding", &texture_usage_storage_binding);
        DOC_VAR(
          "Texture usage flag: texture can be bound as a storage texture to a shader");
        SVAR("int", "Usage_RenderAttachment", &texture_usage_render_attachment);
        DOC_VAR(
          "Texture usage flag: texture can be used as a render attachment, i.e. "
          "written to by a render pass");
        SVAR("int", "Usage_All", &texture_usage_all);
        DOC_VAR("Texture usage flag: all usages enabled");

        // 1D textures currently unsupported
        // static t_CKINT texture_dimension_1d = WGPUTextureDimension_1D;
        static t_CKINT texture_dimension_2d = WGPUTextureDimension_2D;
        // 3D textures currently unsupported
        // static t_CKINT texture_dimension_3d = WGPUTextureDimension_3D;
        // SVAR("int", "Dimension_1D", &texture_dimension_1d);
        SVAR("int", "Dimension_2D", &texture_dimension_2d);
        // SVAR("int", "Dimension_3D", &texture_dimension_3d);

        static t_CKINT texture_format_rgba8unorm  = WGPUTextureFormat_RGBA8Unorm;
        static t_CKINT texture_format_rgba16float = WGPUTextureFormat_RGBA16Float;
        static t_CKINT texture_format_rgba32float = WGPUTextureFormat_RGBA32Float;
        static t_CKINT texture_format_r32float    = WGPUTextureFormat_R32Float;
        // static t_CKINT texture_format_depth24plusstencil8
        //   = WGPUTextureFormat_Depth24PlusStencil8;
        SVAR("int", "Format_RGBA8Unorm", &texture_format_rgba8unorm);
        SVAR("int", "Format_RGBA16Float", &texture_format_rgba16float); // not
        // supported currently
        SVAR("int", "Format_RGBA32Float", &texture_format_rgba32float);
        SVAR("int", "Format_R32Float", &texture_format_r32float);
        // SVAR("int", "Format_Depth24PlusStencil8",
        // &texture_format_depth24plusstencil8);

        // sfun ------------------------------------------------------------------

        SFUN(texture_load_2d_file, SG_CKNames[SG_COMPONENT_TEXTURE], "load");
        ARG("string", "filepath");
        DOC_FUNC("Load a 2D texture from a file");

        SFUN(texture_load_2d_file_with_params, SG_CKNames[SG_COMPONENT_TEXTURE],
             "load");
        ARG("string", "filepath");
        ARG("TextureLoadDesc", "load_desc");
        DOC_FUNC("Load a 2D texture from a file with additional parameters");

        SFUN(texture_load_cubemap, SG_CKNames[SG_COMPONENT_TEXTURE], "load");
        ARG("string", "right");
        ARG("string", "left");
        ARG("string", "top");
        ARG("string", "bottom");
        ARG("string", "back");
        ARG("string", "front");
        DOC_FUNC("Load a cubemap texture from 6 filepaths, one for each face");

        // mfun ------------------------------------------------------------------

        CTOR(texture_ctor);

        CTOR(texture_ctor_with_desc);
        ARG("TextureDesc", "texture_desc");

        MFUN(texture_write, "void", "write");
        ARG("float[]", "pixel_data");
        DOC_FUNC(
          "Convenience function for writing into a texture. Assumes pixel_data is "
          "being written into the texture origin (0,0,0) with a region equal to the "
          "full texture dimensions (width, height, depth) at mip level 0");

        MFUN(texture_write_with_desc, "void", "write");
        ARG("float[]", "pixel_data");
        ARG("TextureWriteDesc", "write_desc");
        DOC_FUNC(
          "Write pixel data to an arbitrary texture region. The input float data is "
          "automatically converted based on the texture format");

        MFUN(texture_get_format, "int", "format");
        DOC_FUNC(
          "Get the texture format (immutable). Returns a value from the "
          "Texture.Format_XXXXX enum, e.g. Texture.Format_RGBA8Unorm");

        MFUN(texture_get_dimension, "int", "dimension");
        DOC_FUNC(
          "Get the texture dimension (immutable). Returns a value from the "
          "Texture.Dimension_XXXXX enum, e.g. Texture.Dimension_2D");

        MFUN(texture_get_width, "int", "width");
        DOC_FUNC("Get the texture width (immutable)");

        MFUN(texture_get_height, "int", "height");
        DOC_FUNC("Get the texture height (immutable)");

        MFUN(texture_get_depth, "int", "depth");
        DOC_FUNC(
          "Get the texture depth (immutable). For a 2D texture, depth corresponds to "
          "the number of array layers (e.g. depth=6 for a cubemap)");

        MFUN(texture_get_usage, "int", "usage");
        DOC_FUNC(
          "Get the texture usage flags (immutable). Returns a bitmask of usage flgas "
          "from the Texture.Usage_XXXXX enum e.g. Texture.Usage_TextureBinding | "
          "Texture.Usage_RenderAttachment. By default, textures are created with ALL "
          "usages enabled");

        MFUN(texture_get_mips, "int", "mips");
        DOC_FUNC(
          "Get the number of mip levels (immutable). Returns the number of mip levels "
          "in the texture.");

        // TODO: specify in WGPUImageCopyTexture where in texture to write to ?
        // e.g. texture.subData()

        END_CLASS();
    }

    ulib_texture_createDefaults(QUERY->ck_api(QUERY));
}

// TextureSampler ------------------------------------------------------------------

Chuck_Object* ulib_texture_ckobj_from_sampler(SG_Sampler sampler, bool add_ref,
                                              Chuck_VM_Shred* shred)
{
    CK_DL_API API = g_chuglAPI;

    Chuck_Object* ckobj = chugin_createCkObj("TextureSampler", add_ref, shred);

    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapU)     = sampler.wrapU;
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapV)     = sampler.wrapV;
    OBJ_MEMBER_INT(ckobj, sampler_offset_wrapW)     = sampler.wrapW;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMin) = sampler.filterMin;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMag) = sampler.filterMag;
    OBJ_MEMBER_INT(ckobj, sampler_offset_filterMip) = sampler.filterMip;

    return ckobj;
}

CK_DLL_CTOR(sampler_ctor)
{
    // default to repeat wrapping and linear filtering
    OBJ_MEMBER_INT(SELF, sampler_offset_wrapU)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(SELF, sampler_offset_wrapV)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(SELF, sampler_offset_wrapW)     = SG_SAMPLER_WRAP_REPEAT;
    OBJ_MEMBER_INT(SELF, sampler_offset_filterMin) = SG_SAMPLER_FILTER_LINEAR;
    OBJ_MEMBER_INT(SELF, sampler_offset_filterMag) = SG_SAMPLER_FILTER_LINEAR;
    OBJ_MEMBER_INT(SELF, sampler_offset_filterMip) = SG_SAMPLER_FILTER_LINEAR;
}

// TextureDesc ---------------------------------------------------------------------

CK_DLL_CTOR(texture_desc_ctor)
{
    OBJ_MEMBER_INT(SELF, texture_desc_format_offset)    = WGPUTextureFormat_RGBA8Unorm;
    OBJ_MEMBER_INT(SELF, texture_desc_dimension_offset) = WGPUTextureDimension_2D;
    OBJ_MEMBER_INT(SELF, texture_desc_width_offset)     = 1;
    OBJ_MEMBER_INT(SELF, texture_desc_height_offset)    = 1;
    OBJ_MEMBER_INT(SELF, texture_desc_depth_offset)     = 1;
    OBJ_MEMBER_INT(SELF, texture_desc_usage_offset)     = WGPUTextureUsage_All;
    // OBJ_MEMBER_INT(SELF, texture_desc_samples_offset) = 1;
    OBJ_MEMBER_INT(SELF, texture_desc_mips_offset) = 0;
}

static SG_TextureDesc ulib_texture_textureDescFromCkobj(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;

    SG_TextureDesc desc = {};
    desc.format = (WGPUTextureFormat)OBJ_MEMBER_INT(ckobj, texture_desc_format_offset);
    desc.dimension
      = (WGPUTextureDimension)OBJ_MEMBER_INT(ckobj, texture_desc_dimension_offset);
    desc.width  = OBJ_MEMBER_INT(ckobj, texture_desc_width_offset);
    desc.height = OBJ_MEMBER_INT(ckobj, texture_desc_height_offset);
    desc.depth  = OBJ_MEMBER_INT(ckobj, texture_desc_depth_offset);
    desc.usage  = OBJ_MEMBER_INT(ckobj, texture_desc_usage_offset);
    // desc.samples        = OBJ_MEMBER_INT(ckobj, texture_desc_samples_offset);
    desc.mips = OBJ_MEMBER_INT(ckobj, texture_desc_mips_offset);

    // validation happens at final layer SG_CreateTexture
    return desc;
}

// TextureWriteDesc -----------------------------------------------------------------

CK_DLL_CTOR(texture_write_desc_ctor)
{
    OBJ_MEMBER_INT(SELF, texture_write_desc_mip_offset)      = 0;
    OBJ_MEMBER_INT(SELF, texture_write_desc_offset_x_offset) = 0;
    OBJ_MEMBER_INT(SELF, texture_write_desc_offset_y_offset) = 0;
    OBJ_MEMBER_INT(SELF, texture_write_desc_offset_z_offset) = 0;
    OBJ_MEMBER_INT(SELF, texture_write_desc_width_offset)    = 1;
    OBJ_MEMBER_INT(SELF, texture_write_desc_height_offset)   = 1;
    OBJ_MEMBER_INT(SELF, texture_write_desc_depth_offset)    = 1;
}

static SG_TextureWriteDesc ulib_texture_textureWriteDescFromCkobj(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;

    SG_TextureWriteDesc desc = {};
    desc.mip                 = OBJ_MEMBER_INT(ckobj, texture_write_desc_mip_offset);
    desc.offset_x = OBJ_MEMBER_INT(ckobj, texture_write_desc_offset_x_offset);
    desc.offset_y = OBJ_MEMBER_INT(ckobj, texture_write_desc_offset_y_offset);
    desc.offset_z = OBJ_MEMBER_INT(ckobj, texture_write_desc_offset_z_offset);
    desc.width    = OBJ_MEMBER_INT(ckobj, texture_write_desc_width_offset);
    desc.height   = OBJ_MEMBER_INT(ckobj, texture_write_desc_height_offset);
    desc.depth    = OBJ_MEMBER_INT(ckobj, texture_write_desc_depth_offset);

    // validation happens at final write

    return desc;
}

// TextureLoadDesc -----------------------------------------------------------------

CK_DLL_CTOR(texture_load_desc_ctor)
{
    OBJ_MEMBER_INT(SELF, texture_load_desc_flip_y_offset)   = false;
    OBJ_MEMBER_INT(SELF, texture_load_desc_gen_mips_offset) = true;
}

static SG_TextureLoadDesc ulib_texture_textureLoadDescFromCkobj(Chuck_Object* ckobj)
{
    CK_DL_API API = g_chuglAPI;

    SG_TextureLoadDesc desc = {};
    desc.flip_y             = OBJ_MEMBER_INT(ckobj, texture_load_desc_flip_y_offset);
    desc.gen_mips           = OBJ_MEMBER_INT(ckobj, texture_load_desc_gen_mips_offset);

    return desc;
}

// Texture -----------------------------------------------------------------

// create default pixel textures and samplers
void ulib_texture_createDefaults(CK_DL_API API)
{
    SG_TextureDesc texture_binding_desc = {};
    texture_binding_desc.usage
      = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;

    SG_TextureWriteDesc texture_write_desc = {};

    // white pixel
    {
        SG_Texture* tex = SG_CreateTexture(&texture_binding_desc, NULL, NULL, true);
        // upload pixel data
        CQ_PushCommand_TextureWrite(tex, &texture_write_desc,
                                    g_builtin_ckobjs.white_pixel_data, API);
        // set global
        g_builtin_textures.white_pixel_id = tex->id;
    }
    //  default render texture (hdr)
    {
        SG_TextureDesc render_texture_desc = {};
        render_texture_desc.usage          = WGPUTextureUsage_RenderAttachment
                                    | WGPUTextureUsage_TextureBinding
                                    | WGPUTextureUsage_StorageBinding;
        render_texture_desc.format = WGPUTextureFormat_RGBA16Float;
        // set global
        g_builtin_textures.default_render_texture_id
          = SG_CreateTexture(&render_texture_desc, NULL, NULL, true)->id;
    }

    { // black pixel
        SG_Texture* tex = SG_CreateTexture(&texture_binding_desc, NULL, NULL, true);
        // upload pixel data
        CQ_PushCommand_TextureWrite(tex, &texture_write_desc,
                                    g_builtin_ckobjs.black_pixel_data, API);
        // set global
        g_builtin_textures.black_pixel_id = tex->id;
    }

    { // magenta pixel
        SG_Texture* tex = SG_CreateTexture(&texture_binding_desc, NULL, NULL, true);
        // upload pixel data
        CQ_PushCommand_TextureWrite(tex, &texture_write_desc,
                                    g_builtin_ckobjs.magenta_pixel_data, API);
        // set global
        g_builtin_textures.magenta_pixel_id = tex->id;
    }

    { // default normal map
        SG_Texture* tex = SG_CreateTexture(&texture_binding_desc, NULL, NULL, true);
        // upload pixel data
        CQ_PushCommand_TextureWrite(tex, &texture_write_desc,
                                    g_builtin_ckobjs.normal_pixel_data, API);
        // set global
        g_builtin_textures.normal_pixel_id = tex->id;
    }

    { // default cube map
        SG_TextureDesc cubemap_desc = {};
        cubemap_desc.depth          = 6; // 6 faces
        cubemap_desc.mips           = 1; // no mips for cubemap

        SG_Texture* tex = SG_CreateTexture(&cubemap_desc, NULL, NULL, true);

        // upload pixel data
        SG_TextureWriteDesc cubemap_write_desc = {};
        cubemap_write_desc.depth               = 6; // 6 faces
        CQ_PushCommand_TextureWrite(tex, &cubemap_write_desc,
                                    g_builtin_ckobjs.default_cube_map_data, API);

        // set global
        g_builtin_textures.default_cubemap_id = tex->id;
    }
}

CK_DLL_CTOR(texture_ctor)
{
    SG_TextureDesc desc = {};
    SG_CreateTexture(&desc, SELF, SHRED, false);
}

CK_DLL_CTOR(texture_ctor_with_desc)
{
    SG_TextureDesc desc = ulib_texture_textureDescFromCkobj(GET_NEXT_OBJECT(ARGS));
    SG_CreateTexture(&desc, SELF, SHRED, false);
}

CK_DLL_MFUN(texture_get_format)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.format;
}

CK_DLL_MFUN(texture_get_dimension)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.dimension;
}

CK_DLL_MFUN(texture_get_width)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.width;
}

CK_DLL_MFUN(texture_get_height)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.height;
}

CK_DLL_MFUN(texture_get_depth)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.depth;
}

CK_DLL_MFUN(texture_get_usage)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.usage;
}

CK_DLL_MFUN(texture_get_mips)
{
    RETURN->v_int = GET_TEXTURE(SELF)->desc.mips;
}

static void ulib_texture_write(SG_Texture* tex, Chuck_ArrayFloat* ck_arr,
                               SG_TextureWriteDesc* desc, Chuck_VM_Shred* SHRED)
{
    CK_DL_API API = g_chuglAPI;

    int num_texels   = desc->width * desc->height * desc->depth;
    int expected_len = num_texels * SG_Texture_numComponentsPerTexel(tex->desc.format);

    { // validation
        char err_msg[256] = {};
        // check offset within image bounds
        if (desc->offset_x + desc->width > tex->desc.width
            || desc->offset_y + desc->height > tex->desc.height
            || desc->offset_z + desc->depth > tex->desc.depth) {
            snprintf(err_msg, sizeof(err_msg),
                     "Texture write region out of bounds. Texture dimensions [%d, %d, "
                     "%d]. Write offsets [%d, %d, %d]. Write region size [%d, %d, %d]",
                     tex->desc.width, tex->desc.height, tex->desc.depth, desc->offset_x,
                     desc->offset_y, desc->offset_z, desc->width, desc->height,
                     desc->depth);
            CK_THROW("TextureWriteOutOfBounds", err_msg, SHRED);
        }

        // check mip level valid
        if (desc->mip >= tex->desc.mips) {
            snprintf(err_msg, sizeof(err_msg),
                     "Invalid mip level. Texture has %d mips, but tried to "
                     "write to mip level %d",
                     tex->desc.mips, desc->mip);
            CK_THROW("TextureWriteInvalidMip", err_msg, SHRED);
        }

        // check ck_array
        int ck_arr_len = API->object->array_float_size(ck_arr);
        if (ck_arr_len < expected_len) {
            snprintf(
              err_msg, sizeof(err_msg),
              "Incorrect number of components in pixel data. Expected %d, got %d",
              expected_len, ck_arr_len);
            CK_THROW("TextureWriteInvalidPixelData", err_msg, SHRED);
        }
    }

    // convert ck array into byte buffer based on texture format
    CQ_PushCommand_TextureWrite(tex, desc, ck_arr, API);
}

CK_DLL_MFUN(texture_write)
{
    SG_Texture* tex          = GET_TEXTURE(SELF);
    SG_TextureWriteDesc desc = {};
    desc.width               = tex->desc.width;
    desc.height              = tex->desc.height;
    desc.depth               = tex->desc.depth;

    ulib_texture_write(tex, GET_NEXT_FLOAT_ARRAY(ARGS), &desc, SHRED);
}

CK_DLL_MFUN(texture_write_with_desc)
{
    SG_Texture* tex          = GET_TEXTURE(SELF);
    Chuck_ArrayFloat* ck_arr = GET_NEXT_FLOAT_ARRAY(ARGS);
    SG_TextureWriteDesc desc
      = ulib_texture_textureWriteDescFromCkobj(GET_NEXT_OBJECT(ARGS));

    ulib_texture_write(tex, ck_arr, &desc, SHRED);
}

SG_Texture* ulib_texture_load(const char* filepath, SG_TextureLoadDesc* load_desc,
                              Chuck_VM_Shred* shred)
{
    int width, height, num_components;
    if (!stbi_info(filepath, &width, &height, &num_components)) {
        log_warn("Could not load texture file '%s'", filepath);
        log_warn(" |- Reason: %s", stbi_failure_reason());
        log_warn(" |- Defaulting to magenta texture");

        // on failure return magenta texture
        return SG_GetTexture(g_builtin_textures.magenta_pixel_id);
    }

    SG_TextureDesc desc = {};
    desc.width          = width;
    desc.height         = height;
    desc.dimension      = WGPUTextureDimension_2D;
    desc.format         = WGPUTextureFormat_RGBA8Unorm;
    desc.usage          = WGPUTextureUsage_All;

    SG_Texture* tex = SG_CreateTexture(&desc, NULL, shred, false);

    CQ_PushCommand_TextureFromFile(tex, filepath, load_desc);

    return tex;
}

CK_DLL_SFUN(texture_load_2d_file)
{
    SG_TextureLoadDesc load_desc = {};
    SG_Texture* tex
      = ulib_texture_load(API->object->str(GET_NEXT_STRING(ARGS)), &load_desc, SHRED);
    RETURN->v_object = tex ? tex->ckobj : NULL;
}

CK_DLL_SFUN(texture_load_2d_file_with_params)
{
    const char* filepath = API->object->str(GET_NEXT_STRING(ARGS));
    SG_TextureLoadDesc load_desc
      = ulib_texture_textureLoadDescFromCkobj(GET_NEXT_OBJECT(ARGS));

    SG_Texture* tex = ulib_texture_load(filepath, &load_desc, SHRED);

    RETURN->v_object = tex ? tex->ckobj : NULL;
}

// load cubemap
SG_Texture* ulib_texture_load_cubemap(const char* right_face, const char* left_face,
                                      const char* top_face, const char* bottom_face,
                                      const char* back_face, const char* front_face,
                                      SG_TextureLoadDesc* load_desc,
                                      Chuck_VM_Shred* shred)
{
    const char* filepaths[6]
      = { right_face, left_face, top_face, bottom_face, back_face, front_face };

    int cubemap_width{}, cubemap_height{};
    for (int i = 0; i < 6; i++) {
        int width{}, height{};
        if (!stbi_info(filepaths[i], &width, &height, NULL)) {
            log_warn("Could not load texture file '%s'", filepaths[i]);
            log_warn(" |- Reason: %s", stbi_failure_reason());
            log_warn(" |- Defaulting to magenta texture");

            // on failure return default cubemap
            return SG_GetTexture(g_builtin_textures.default_cubemap_id);
        }

        if (cubemap_width == 0) cubemap_width = width;
        if (cubemap_height == 0) cubemap_height = height;

        // check if all faces have same dimensions
        if (cubemap_width != width || cubemap_height != height) {
            log_warn(
              "Cubemap faces have different dimensions %dx%d vs %dx%d on face %d",
              cubemap_width, cubemap_height, width, height, i);
            log_warn(" |- Defaulting to magenta texture");

            // on failure return default cubemap
            return SG_GetTexture(g_builtin_textures.default_cubemap_id);
        }
    }

    SG_TextureDesc desc = {};
    desc.width          = cubemap_width;
    desc.height         = cubemap_height;
    desc.depth          = 6; // 6 faces
    desc.dimension      = WGPUTextureDimension_2D;
    desc.format         = WGPUTextureFormat_RGBA8Unorm;
    desc.usage          = WGPUTextureUsage_All;
    desc.mips           = 1;

    SG_Texture* tex = SG_CreateTexture(&desc, NULL, shred, false);

    CQ_PushCommand_CubemapTextureFromFile(tex, load_desc, right_face, left_face,
                                          top_face, bottom_face, back_face, front_face);

    return tex;
}

CK_DLL_SFUN(texture_load_cubemap)
{
    SG_TextureLoadDesc load_desc = {};
    load_desc.gen_mips           = false; // don't generate mips for cubemap
    const char* right_face       = API->object->str(GET_NEXT_STRING(ARGS));
    const char* left_face        = API->object->str(GET_NEXT_STRING(ARGS));
    const char* top_face         = API->object->str(GET_NEXT_STRING(ARGS));
    const char* bottom_face      = API->object->str(GET_NEXT_STRING(ARGS));
    const char* back_face        = API->object->str(GET_NEXT_STRING(ARGS));
    const char* front_face       = API->object->str(GET_NEXT_STRING(ARGS));

    RETURN->v_object
      = ulib_texture_load_cubemap(right_face, left_face, top_face, bottom_face,
                                  back_face, front_face, &load_desc, SHRED)
          ->ckobj;
}