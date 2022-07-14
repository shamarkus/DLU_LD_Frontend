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
    //Uncover Internal Parameters
    document.querySelector('#parameter_div').removeAttribute('hidden');
}

Date.prototype.addSeconds = function(seconds){
    let date = new Date(this.valueOf());
    date.setSeconds(date.getSeconds() + seconds);
    return date;
}

function compare(a,b){
    if (a.time<b.time) {
        return -1;
    }
    if (b.time<a.time) {
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

            times.push({time: endTime, index: i});
            times.push({time: startTime, index: i});
            timesStringArr.push({startTime: startTime,endTime: endTime,index: i});
        }
        times.sort(compare);
        getTimeIntervals(addToSelectTimeList);
    });
}

function getTimeIntervals(myCallback){
    let startEndContainer = new Array();
    let startTimes = new Array();
    let endTimes = new Array();
    let curDate = times[0].time.addSeconds(60);

    for(let i=0;i<times.length;i++){
        if(i !== 0 && startEndContainer.length === 0 && ((times[i].time-times[i-1].time)/1000) > 1 ){
            console.log("Consecutive files are not time-continuous!");
            curDate = times[i].time.addSeconds(60);
        }
        while(curDate < times[i].time){
            startTimes.push(`${curDate.yyyymmdd()}`);
            endTimes.push(`${curDate.yyyymmdd()}`);
            curDate = curDate.addSeconds(60);
        }

        if(!startEndContainer.includes(times[i].index)){
            startEndContainer.push(times[i].index);
            startTimes.push(`SOF ${times[i].index+1} : ${times[i].time.yyyymmdd()}`);
        }
        else{
            startEndContainer.splice(startEndContainer.indexOf(times[i].index));
            endTimes.push(`EOF ${times[i].index+1} : ${times[i].time.yyyymmdd()}`);
        }
    }
    myCallback(startTimes,endTimes);
}
Date.prototype.yyyymmdd = function() {
  let mm = this.getMonth() + 1; // getMonth() is zero-based
  let dd = this.getDate();
  return `${this.getFullYear()}-${(mm>9 ? '' : '0')+mm}-${(dd>9 ? '' : '0')+dd} ${this.getHours()}:${this.getMinutes()}:${this.getSeconds()}`;
};
function addToSelectTimeList(startTimes,endTimes){
    let selectStart = document.querySelector("#start");
    let selectEnd = document.querySelector("#end");
    //startTimes length == endTimes length
    for(let i = 0;i<startTimes.length;i++){
        let optS = document.createElement('option');
        let optE = document.createElement('option');
        optS.value = i;
        optE.value = i;
        optS.textContent = startTimes[i];
        selectStart.appendChild(optS);
        optE.textContent = endTimes[i];
        selectEnd.appendChild(optE);
    }
    selectStart.value = 0;
    selectEnd.value = selectEnd.length-1;
}
//sel is End Time Select
function setStartTimes(sel){
    let selectStart = document.querySelector("#start");
    for(let i = 0;i<selectStart.length;i++){
        if(i >= (sel.selectedIndex+1)){
            selectStart.options[i].disabled = true;
        }
        else{
            selectStart.options[i].disabled = false;
        }
    }
}

//sel is Start Time Select
function setEndTimes(sel){
    let selectEnd = document.querySelector("#end");
    for(let i = 0;i<selectEnd.length;i++){
        if(i >= (sel.selectedIndex)){
            selectEnd.options[i].disabled = false;
        }
        else{
            selectEnd.options[i].disabled = true;
        }
    }
}

function callTables(sel){
    let content;
    if(!sel.selectedIndex){
        content = ATO;
    }
    else{
        content = ATP;
    }
    
    let paramSel = document.querySelector('#sel');
    paramSel.textContent = '';
    let lines = content.split('\n');
    lines.forEach((line) => {
        let opt = document.createElement('option');
        opt.textContent = (line.split('\t'))[0];
        opt.value = (line.split('\t'))[1];
        paramSel.appendChild(opt);
    });
    Object.values(document.querySelector('#parameter_div').children).forEach(function(child){
        child.removeAttribute('hidden');
    });
}

function showResults(val){
    let res = document.querySelector('#sel');
    let ATindex = document.querySelector('#ATsel').selectedIndex;
    res.textContent='';
    autocompleteMatch(val,ATindex).forEach(line => {
        let opt = document.createElement('option');
        opt.textContent = (line.split('\t'))[0];
        opt.value = (line.split('\t'))[1];
        res.appendChild(opt);
    });
}

function autocompleteMatch(input,ATindex) {
    if (input == '') {
        callTables(document.querySelector("#ATsel"));
        return [];
    }
    let content;
    if(!ATindex){
        content = ATO.split('\n');
    }
    else{
        content = ATP.split('\n');
    }
    
    let reg = new RegExp(input);
    return content.filter(function(term) {
        if (term.match(reg)) {
            return term;
        }
    });
}

function transferRows(curSel,receiveSel){
    receiveSel.appendChild(curSel.options[curSel.selectedIndex]);
}