window.addEventListener("load", () => {
	const sounds = document.querySelectorAll('audio');
	const pads   = document.querySelectorAll('.pads div');
	const visual = document.querySelector('.visual');
	const colors = [
	    "crimson",
	    "#4286f4",
	    "#4ECDC4",
	    "#355C7D",
	    "#606bd3",
	    "#11998e",
	    "#c31432",
	    "#60d394"
	  ];


	pads.forEach( (pad , index)=> {
		pad.addEventListener('click', ()=>{
			
			sounds[index].currentTime = 0;
			// PLAY THE SOUND
			sounds[index].play();
			// MAKE A BUBBLE
			makeBubbles(index);
		});
	})

	// FUNCTIONALITY MAKE AN EFFECT
	const makeBubbles = (index) => {
		const bubble = document.createElement('div');
		visual.append(bubble);
		bubble.style.backgroundColor = colors[index];
		bubble.addEventListener('animationend', function(){
			visual.removeChild(this);
		})
	}
});