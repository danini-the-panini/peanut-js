if (typeof mergeInto !== 'undefined') mergeInto(LibraryManager.library, {
  js_init: function(width, height, audioSamples, audioSampleRate) {
    canvasElement.width = width;
    canvasElement.height = height;
    window.AUDIO_SAMPLES = audioSamples;
    window.AUDIO_SAMPLE_RATE = audioSampleRate;
  },
  draw_line: function(pixels, width, line) {
    let unsafe = new Uint8Array(Module.HEAPU8.buffer, pixels, width);
    let data = new Uint8Array(unsafe);
    let imageData = ctx.createImageData(width, 1);
    for (let x = 0; x < width; x++) {
      let c = COLORS[data[x] & 3];
      imageData.data[x * 4]     = c.r;
      imageData.data[x * 4 + 1] = c.g;
      imageData.data[x * 4 + 2] = c.b;
      imageData.data[x * 4 + 3] = 255;
    }
    ctx.putImageData(imageData, 0, line);
  },
  js_play_audio(stream, len) {
    let unsafe = new Uint8Array(Module.HEAPU8.buffer, stream, len);
    let u8 = new Uint8Array(unsafe);
    let data = new Int16Array(u8.buffer);
    let channels = 2;
    let buffer = audioCtx.createBuffer(channels, AUDIO_SAMPLES, AUDIO_SAMPLE_RATE);
    for (let i = 0; i < data.length; i+=channels) {
      for (let c = 0; c < channels; c++) {
        let channel = buffer.getChannelData(c);
        let x = data[i+c] / 32768.0;
        if (x < -1.0) x = -1.0;
        else if (x > 1.0) x = 1.0;
        channel[i/channels] = x
      }
    }
    let audioSource = audioCtx.createBufferSource();
    audioSource.buffer = buffer;
    audioSource.connect(audioCtx.destination);
    audioSource.start();
  }
});
