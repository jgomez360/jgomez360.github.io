var initialized = false;

Pebble.addEventListener("ready", function() {
	console.log("ready called!");
	initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
	console.log("showing configuration");
	Pebble.openURL('https://dl.dropboxusercontent.com/u/12309605/js-configure-demo-master/configurable1_test.html');
//	Pebble.openURL('https://drive.google.com/open?id=0By6_iohjaHzLRmhpV01YdlNpVXM');
});

Pebble.addEventListener("appmessage", function(e) {
	console.log("Got message: " + JSON.stringify(e));
});
						 
Pebble.addEventListener("webviewclosed", function(e) {
	console.log("configuration closed");
	// webview closed
	//var state = 0;
	var w = JSON.parse(decodeURIComponent(e.response));
//	console.log("Workout = " + JSON.stringify(w) + " NumEx:" + w.exercises.length);
	console.log("Workout = " + JSON.stringify(w));
	
	sendWorkout = function(state, eIndex) {
		switch (state) {
			case 0:
				// Send Workout Name
				//console.log("Sending EName " + w.exercises[index].name);
				Pebble.sendAppMessage( { "2":w.exercises[eIndex].name } ,
					function(e) { sendWorkout(1, eIndex); },
					function (e) { console.log("Failure"); }
				);
				break;
			case 1:
				// Send Rest Timer Value
				//console.log("Sending ERTime " + w.exercises[index].restTime);
				Pebble.sendAppMessage( { "3":w.exercises[eIndex].restTime } ,
					//function(e) { sendWorkout(2, eIndex); },
					function(e) { sendWorkout(2, eIndex); },
					function (e) { console.log("Failure"); }
				);
				break;
			case 2:
				// Send Rep Values
				Pebble.sendAppMessage( { "4":JSON.stringify(w.exercises[eIndex].reps) } ,
					function(e) { sendWorkout(3, eIndex); },
					function (e) { console.log("Failure"); }
				);
				break;
			case 3:
				// Send Load Values
				//console.log("Sending EReps " + w.exercises[eIndex].reps);
//				Pebble.sendAppMessage( { "5":w.exercises[eIndex].loads } ,
				Pebble.sendAppMessage( { "5":JSON.stringify(w.exercises[eIndex].loads) } ,
					function(e) {
						if ((eIndex + 1) < w.exercises.length) { 
							sendWorkout(0, eIndex + 1);
						} else {
							Pebble.sendAppMessage( { "6":"Done" } ,
								function(e) { console.log("Finished Sending Workout"); },
								function (e) { console.log("Failure"); }
							);
						}
					},
					function (e) {
						console.log("Failure");
					}
				);
				break;
			default:
				break;
		}
	}

	// Let the pebble app know how much data we want to send.
	Pebble.sendAppMessage( { "0": w.name } ,
		function (e) {
			Pebble.sendAppMessage( { "1":w.exercises.length } ,
				function(e) { 
					sendWorkout(0, 0); /* Begin sending exercises */
				},
				function (e) { console.log("Failure"); }
			);
		},
		function (e) { console.log("Failure"); }
	);
});

//function sendAppMessageSuccess(e) {
//	console.log("Successfully delivered message with transactionId=" + e.data.transactionId);		
//}

//function sendAppMessageFailure(e) {
//	console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " Error is: " + e.error.message);
//}

