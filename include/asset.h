// asset.h

typedef enum Asset_type {
  ASSET_TEXTURE,
  ASSET_SHADER,
  ASSET_AUDIO,
};

struct Asset {
  Asset_type type;
  union {
    i32 texture_id;
    i32 shader_program;
    struct {
      float* audio_buffer;
      i32 audio_buffer_size;
      i8 channel_count;
    } audio_info;
  } data;
};
