//https://stackoverflow.com/questions/16215771/how-to-open-select-file-dialog-via-js
//
//
//
//
'use strict';
const addedFiles = new Array();
const times = new Array();
const timesStringArr = new Array();

// Restart the entire webpage
function restart() {
    window.location.reload(true);
}

//Open file explorer
//Handles unique files
function add_file() {
    let input = document.createElement('input');
    input.type = 'file';
    input.multiple = 'multiple';
    input.onchange = e => {
        for(let i=0;i<e.target.files.length;i++){
            if(addedFiles.filter(f => f.name === e.target.files[i].name).length === 0){
                addedFiles.push(e.target.files[i]);
            }
        }
        document.querySelector('#remove_file').disabled = false;
        document.querySelector('#upload').disabled = false;
        printAddedFiles();
    }
    input.click();
}

//Prints added Filenames based on addedFiles Array
function printAddedFiles() {
    let addedFilesText = document.querySelector('#added_files');
    if(addedFiles.length){
        addedFilesText.textContent = `${addedFiles[0].name}`;
        for(let i=1;i<addedFiles.length;i++){
            addedFilesText.textContent += `,`
            addedFilesText.textContent += ` ${addedFiles[i].name}`;
        }
    }
    else{
        addedFilesText.textContent = '';
        document.querySelector('#remove_file').disabled = true;
        document.querySelector('#upload').disabled = true;
    }
}

//Removes most recently added file
//Toggles functionality of Remove_File Button
function remove_file() {
    addedFiles.pop();
    printAddedFiles();
}

//Uploads files for processing
//Uncovers the Internal Parameter Fields
function Upload(b){
    document.querySelector('#add_file').disabled = true;
    document.querySelector('#remove_file').disabled = true;
    b.disabled = true;

    //Read file to get time layouts
    getStartEndTimes();
    //Convert to epoch --> Add to list
    addToSelectTimeList();
    //Uncover Internal Parameters
    document.querySelector('#parameter_div').removeAttribute('hidden');
}

Date.prototype.addSeconds = function(seconds){
    let date = new Date(this.valueOf());
    date.setDate(date.getDate() + seconds);
    return date;
}

function addToSelectTimeList(){
    
}
function compare(a,b){
    if (a<b) {
        return -1;
    }
    if (b<a) {
        return 1;
    }
    return 0;
}

function readFileAsText(file){
    return new Promise(function(resolve,reject){
        let fr = new FileReader();
        fr.onload = function(){
            resolve(fr.result);
        };
        fr.onerror = function(){
            reject(fr);
        };
        fr.readAsText(file);
    });
}

function getStartEndTimes(){
    let readers = [];
    for(let i=0;i<addedFiles.length;i++){
        readers.push(readFileAsText(addedFiles[i]));
    }
    Promise.all(readers).then((content) => {
        for(let i=0;i<content.length;i++){
            let lines = content[i].split('\n');
            let startTime = `${(lines[1].split('\t'))[0]} ${(lines[1].split('\t'))[1]}`;
            let endTime = `${(lines[lines.length-2].split('\t'))[0]} ${(lines[lines.length-2].split('\t'))[1]}`;

            startTime = new Date(startTime.slice(0,-4));
            endTime = new Date(endTime.slice(0,-4));

            times.push(endTime);
            times.push(startTime);
            timesStringArr.push({startTime: startTime,endTime: endTime,index: i});

        }
        times.sort(compare);
    });
}