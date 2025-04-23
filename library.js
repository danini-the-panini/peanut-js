if (typeof mergeInto !== 'undefined') mergeInto(LibraryManager.library, {
  js_init: function(width, height) {
    canvasElement.width = width;
    canvasElement.height = height;
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
  }
});
