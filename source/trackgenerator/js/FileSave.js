function fileLoad() {
	console.log("Loading scenario");

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