
const recorder = document.getElementById('recorder');

const player = document.getElementById('player');
recorder.addEventListener('change', function(e) {
  const file = e.target.files[0];
  const url = URL.createObjectURL(file);
  //do work


  //work done
  player.src = url;
  document.getElementById('player').play();
});