function add_file() {
    //
    num_files += +1;

    var space = document.createElement("br");
    space.setAttribute("id", "fileUpload_space" + num_files)
    document.getElementById("upload_buttons").appendChild(space);

    var file = document.createElement("input");
    file.setAttribute("type", "file");
    file.setAttribute("id", "fileUpload" + num_files)
    file.setAttribute("class", "fileUpload");
    document.getElementById("upload_buttons").appendChild(file);
}

// Remove a file upload button. Allows user to upload one less file. Removes last button.
function remove_file() {
    // Remove file upload button
    var file = document.getElementById("fileUpload" + num_files);
    d3.select(file).remove();

    // Remove file upload button's spacing
    var space = document.getElementById("fileUpload_space" + num_files);
    d3.select(space).remove();

    // Decrement the number of files
    num_files -= 1;

}

// Restart the entire webpage
function restart() {
    //
    window.location.reload(true);
}
