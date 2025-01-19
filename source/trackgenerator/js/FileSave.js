function fileLoad() {
	console.log("Loading scenario");

	let input = document.createElement("input");
	input.id = "fileupload";
	input.type = "file";
	input.accept = ".etmap";

	input.addEventListener("change", (aEvent) => {
		let fileread = new FileReader();
		fileread.addEventListener("loadend", (aEvent) => {
			scenarioDeserialize(aEvent.target.result);
		});
		fileread.readAsArrayBuffer(input.files[0]);
	});

	input.click();
}
function fileSave() {
	console.log("Saving scenario");

	let scenario = scenarioSerialize();

	console.log(scenario);

	let url = URL.createObjectURL(scenario);
	let elem = document.createElement("a");
	elem.href = url;
	let name = document.getElementById("scenarioname").value;
	elem.download = ((name.length == 0) ? "scenario" : name)+".etmap";
	elem.click();

	URL.revokeObjectURL(url);
}

//called from Line.js
function lineSave(ablob) {
	console.log("Saving line");

	console.log(ablob);

	let url = URL.createObjectURL(ablob);
	let elem = document.createElement("a");
	let name = document.getElementById("linename").value;
	elem.download = ((name.length == 0) ? "line" : name)+".etscr";
	elem.click();

	URL.revokeObjectURL(url);
}