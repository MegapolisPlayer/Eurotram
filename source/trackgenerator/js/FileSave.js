function fileLoad() {
	console.log("Loading scenario");

	let input = document.createElement("input");
	input.id = "fileupload";
	input.type = "file";
	input.accept = ".etmap";

	input.addEventListener("change", (e) => {
		let fileread = new FileReader();
		fileread.addEventListener("loadend", (e) => {
			scenarioDeserialize(e.target.result);
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