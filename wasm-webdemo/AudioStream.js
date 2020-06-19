
var startRecordingButton = document.getElementById("startRecordingButton");
var stopRecordingButton = document.getElementById("stopRecordingButton");
var playButtonR = document.getElementById("playButtonR");
var playButtonD = document.getElementById("playButtonD");
var downloadButton = document.getElementById("downloadButton");


var leftchannel = [];
var rightchannel = [];
var denoisedchannel = [];

var recorder = null;
var recordingLength = 0;
var volume = null;
var mediaStream = null;
var sampleRate = 44100;
var context = null;

var blobIn = null;
var blobOut = null;


startRecordingButton.addEventListener("click", function () {
  // Initialize recorder
  navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.msGetUserMedia;
  navigator.getUserMedia(
    {
      audio: true
    },
    function (e) {
      console.log("user consent");

      // creates the audio context
      window.AudioContext = window.AudioContext || window.webkitAudioContext;
      context = new AudioContext();

      // creates an audio node from the microphone incoming stream
      mediaStream = context.createMediaStreamSource(e);

      // https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createScriptProcessor
      // bufferSize: the onaudioprocess event is called when the buffer is full
      var bufferSize = 2048;
      var numberOfInputChannels = 1;
      var numberOfOutputChannels = 1;
      if (context.createScriptProcessor) {
        recorder = context.createScriptProcessor(bufferSize, numberOfInputChannels, numberOfOutputChannels);
      } else {
        recorder = context.createJavaScriptNode(bufferSize, numberOfInputChannels, numberOfOutputChannels);
      }

      recorder.onaudioprocess = function (e) {
        leftchannel.push(new Float32Array(e.inputBuffer.getChannelData(0)));
        console.log(e.inputBuffer.getChannelData(0));

        // rightchannel.push(new Float32Array(e.inputBuffer.getChannelData(1)));
        wasm_noise_cancel(e.inputBuffer);

        recordingLength += bufferSize;
      }

      // we connect the recorder
      mediaStream.connect(recorder);
      recorder.connect(context.destination);
    },
    function (e) {
      console.error(e);
    });
});

stopRecordingButton.addEventListener("click", function () {

  // stop recording
  recorder.disconnect(context.destination);
  mediaStream.disconnect(recorder);
  blobIn = writeWAV(leftchannel, rightchannel);

});

function writeWAV(left, right) {
  // we flat the left and right channels down
  // Float32Array[] => Float32Array
  var leftBuffer = flattenArray(left, recordingLength);
  var rightBuffer = flattenArray(right, recordingLength);
  // we interleave both channels together
  // [left[0],right[0],left[1],right[1],...]
  var interleaved = interleave(leftBuffer, rightBuffer);

  // we create our wav file
  var buffer = new ArrayBuffer(44 + interleaved.length * 2);
  var view = new DataView(buffer);

  // RIFF chunk descriptor
  writeUTFBytes(view, 0, 'RIFF');
  view.setUint32(4, 44 + interleaved.length * 2, true);
  writeUTFBytes(view, 8, 'WAVE');
  // FMT sub-chunk
  writeUTFBytes(view, 12, 'fmt ');
  view.setUint32(16, 16, true); // chunkSize
  view.setUint16(20, 1, true); // wFormatTag
  view.setUint16(22, 2, true); // wChannels: stereo (2 channels)
  view.setUint32(24, sampleRate, true); // dwSamplesPerSec
  view.setUint32(28, sampleRate * 4, true); // dwAvgBytesPerSec
  view.setUint16(32, 4, true); // wBlockAlign
  view.setUint16(34, 16, true); // wBitsPerSample
  // data sub-chunk
  writeUTFBytes(view, 36, 'data');
  view.setUint32(40, interleaved.length * 2, true);

  // write the PCM samples
  var index = 44;
  var volume = 1;
  for (var i = 0; i < interleaved.length; i++) {
    view.setInt16(index, interleaved[i] * (0x7FFF * volume), true);
    index += 2;
  }

  // our final blob
  return new Blob([view], { type: 'audio/wav' });
}

playButtonR.addEventListener("click", function () {
  if (blobIn == null) {
    return;
  }

  var url = window.URL.createObjectURL(blobIn);
  var audio = new Audio(url);
  audio.play();
});

playButtonD.addEventListener("click", function () {
  if (blobOut == null) {
    return;
  }

  var url = window.URL.createObjectURL(blobOut);
  var audio = new Audio(url);
  audio.play();
});

downloadButton.addEventListener("click", function (inBlob) {
  if (inBlob == null) {
    return;
  }

  var url = URL.createObjectURL(inBlob);

  var a = document.createElement("a");
  document.body.appendChild(a);
  a.style = "display: none";
  a.href = url;
  a.download = "sample.wav";
  a.click();
  window.URL.revokeObjectURL(url);
});

function flattenArray(channelBuffer, recordingLength) {
  var result = new Float32Array(recordingLength);
  var offset = 0;
  for (var i = 0; i < channelBuffer.length; i++) {
    var buffer = channelBuffer[i];
    result.set(buffer, offset);
    offset += buffer.length;
  }
  return result;
}

function interleave(leftChannel, rightChannel) {
  var length = leftChannel.length + rightChannel.length;
  var result = new Float32Array(length);

  var inputIndex = 0;

  for (var index = 0; index < length;) {
    result[index++] = leftChannel[inputIndex]
    result[index++] = rightChannel[inputIndex];
    inputIndex++;
  }
  return result;
}

function writeUTFBytes(view, offset, string) {
  for (var i = 0; i < string.length; i++) {
    view.setUint8(offset + i, string.charCodeAt(i));
  }
}


function wasm_noise_cancel(buffer) {
  let wasmMemInP = Module._malloc(buffer.length);

  let wasmMemIn = new Float32Array(wasmMemory.buffer, wasmMemInP, buffer.length);
  wasmMemIn.set(buffer.getChannelData(0));

  denoise_proc_mono = Module.cwrap('denoise_proc_mono', 'number', ['number', 'number']);
  denoise_proc = Module.cwrap('denoise_proc', '', ['number', 'number', 'number', 'number']);
  let x= denoise_proc_mono(wasmMemInP, buffer.length);


  // rn_noise_cancel = Module.cwrap('rn_noise_cancel', 'number', ['number', 'number', 'number']);
  // let x= rn_noise_cancel(wasmMemInP, wasmMemInP, buffer.length);

  // denoise_proc = Module.cwrap('denoise_proc', '', ['number', 'number', 'number', 'number']);
  // let memOutP = denoise_proc(wasmMemInP, buffer.sampleRate, buffer.numberOfChannels, buffer.length)
  let wasmMemOut = new Float32Array(wasmMemory.buffer, x, buffer.length);
  buffer.copyToChannel(wasmMemOut, 0, 0)
  /*
    let myJSArray = new Float32Array();
    let myWasmArrayPtr = Module._malloc(buffer.length);
    let myWASMArray = new Float32Array(wasmMemory.buffer, myWasmArrayPtr, buffer.length);
    myWASMArray.set(buffer.getChannelData(0));
    denoise_proc = Module.cwrap('denoise_proc', '', ['number', 'number', 'number', 'number']);
    let memOutP = denoise_proc(myWasmArrayPtr, buffer.length, buffer.sampleRate, buffer.numberOfChannels)s
    let myWASMOutArray = new Float32Array(wasmMemory.buffer, memOutP, buffer.length);
    myJSArray.set(myWASMOutArray);
    buffer.copyToChannel(myJSArray, 0, 0)
  */
}





