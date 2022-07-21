'use strict';
const addedFiles = new Array();
const times = new Array();
const timesStringArr = new Array();
let analyzeParams = new Array();

// Restart the entire webpage
function restart() {
    window.location.reload(true);
}

//Open file explorer
//Handles unique files
function add_file() {
    let input = document.createElement('input');
    let buttonsDiv = document.querySelector('#upload_buttons');
    let addedFilesSel = document.querySelector('#addedFilenames');
    input.type = 'file';
    input.multiple = 'multiple';
    input.onchange = e => {
        while(buttonsDiv.children.length > 8){
            buttonsDiv.removeChild(buttonsDiv.children[4]);
        }
        let dupText = document.createElement('p');
        dupText.style = 'color:red; display:inline;';
        dupText.textContent = `Error: Failed To Upload File - Conflicting File Names `;
        for(let i=0;i<e.target.files.length;i++){
            const dups = addedFiles.filter(f => f.name === e.target.files[i].name);
            if(dups.length === 0){
                let opt = document.createElement('option');
                addedFiles.push(e.target.files[i]);
                opt.textContent = e.target.files[i].name;
                addedFilesSel.appendChild(opt);
            }
            else{
                dupText.textContent = `${dupText.textContent} --- ${dups[0].name}`;
                buttonsDiv.insertBefore(dupText,buttonsDiv.children[4]);
            }
        }
        document.querySelector('#remove_file').disabled = false;
        document.querySelector('#upload').disabled = false;
    }
    input.click();
    input.remove();
}

//Removes most recently added file
//Toggles functionality of Remove_File Button
function remove_file() {
    let selectedFile = document.querySelector('#addedFilenames');
    if(selectedFile.selectedIndex !== -1){
        let opt = selectedFile.children[selectedFile.selectedIndex];
        addedFiles.splice(selectedFile.selectedIndex,1);
        selectedFile.removeChild(opt);
    }
    if(!addedFiles.length){
        document.querySelector('#remove_file').disabled = true;
        document.querySelector('#upload').disabled = true;
        return;
    }
}

//Uploads files for processing
//Uncovers the Internal Parameter Fields
function Upload(b){
    document.querySelector('#add_file').disabled = true;
    document.querySelector('#remove_file').disabled = true;
    document.querySelector('#addedFilenames').disabled = true;
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
            
            startTime = new Date(startTime.slice(0,-4)+' UTC');
            endTime = new Date(endTime.slice(0,-4)+' UTC');
            times.push({time: startTime, index: i});
            times.push({time: endTime, index: i});
            timesStringArr.push({startTime: startTime,endTime: endTime,index: i});
        }
        times.sort(compare);
        getTimeIntervals(addToSelectTimeList);
    });
}

function getTimeIntervals(myCallback){
    let startEndContainer = new Array();
    let startTimes = new Array();
    let curDate = times[0].time.addSeconds(60);

    for(let i=0;i<times.length;i++){
        if(i !== 0 && startEndContainer.length === 0 ){
            curDate = times[i].time.addSeconds(60);
            if (((times[i].time-times[i-1].time)/1000) > 1 ){
                startTimes.push('NON TIME-CONTINUOUS INTERVAL');
            }
            else{
                startTimes.push('CONTINUOUS TIME-INTERVAL');
            }
        }
        while(curDate < times[i].time){
            if(times.filter(e => e.time.getTime() === curDate.getTime()).length === 0){
                startTimes.push(curDate.yyyymmdd());
            }
            curDate = curDate.addSeconds(60);
        }

        if(!startEndContainer.includes(times[i].index)){
            startEndContainer.push(times[i].index);
            startTimes.push(`SOF ${times[i].index+1} : ${times[i].time.yyyymmdd()}`);
        }
        else{
            startEndContainer.splice(startEndContainer.indexOf(times[i].index),1);
            startTimes.push(`EOF ${times[i].index+1} : ${times[i].time.yyyymmdd()}`);
        }
    }
    myCallback(startTimes);
}
Date.prototype.yyyymmdd = function() {
  let mm = this.getMonth() + 1; // getMonth() is zero-based
  let dd = this.getDate();
  let hh = this.getHours();
  let MM = this.getMinutes();
  let SS = this.getSeconds();
  return `${this.getFullYear()}-${(mm>9 ? '' : '0')+mm}-${(dd>9 ? '' : '0')+dd} ${(hh>9 ? '' : '0')+hh}:${(MM>9 ? '' : '0')+MM}:${(SS>9 ? '' : '0')+SS}`;
};
function addToSelectTimeList(startTimes){
    let selectStart = document.querySelector("#start");
    let selectEnd = document.querySelector("#end");
    for(let i = 0;i<startTimes.length;i++){
        let opt = document.createElement('option');
        opt.value = i;
        opt.textContent = startTimes[i];
        if(i !== startTimes.length-1){
            selectStart.appendChild(opt);
        }
        if(startTimes[i-1] === 'CONTINUOUS TIME-INTERVAL'){
            selectStart.removeChild(selectStart.children[i-1]);
            selectStart.removeChild(selectStart.children[i-2]);
            selectEnd.removeChild(selectEnd.children[i-2]);
            continue;
        }
        else if(startTimes[i-1] ==='NON TIME-CONTINUOUS INTERVAL'){
            selectStart.children[i-1].disabled = true;
            selectEnd.children[i-2].disabled = true;
            selectStart.removeChild(selectStart.children[i-2]);
            continue;
        }
        if(i){
            selectEnd.appendChild(opt.cloneNode(true));
        }
    }
    selectStart.value = 0;
    selectEnd.value = selectEnd.options[selectEnd.options.length-1].value;
}
//sel is End Time Select
function setStartTimes(sel){
    let selectStart = document.querySelector("#start");
    for(let i = 0;i<selectStart.length;i++){
        if(i >= (sel.selectedIndex+1)){
            selectStart.options[i].disabled = true;
        }
        else {
            selectStart.options[i].textContent !=='NON TIME-CONTINUOUS INTERVAL' ? selectStart.options[i].disabled = false : 1;
        }
    }
}

//sel is Start Time Select
function setEndTimes(sel){
    let selectEnd = document.querySelector("#end");
    for(let i = 0;i<selectEnd.length;i++){
        if(i >= (sel.selectedIndex)){
            selectEnd.options[i].textContent !=='NON TIME-CONTINUOUS INTERVAL' ? selectEnd.options[i].disabled = false : 1;
        }
        else{
            selectEnd.options[i].disabled = true;
        }
    }
}

//Initializes all tables with the corresponding parameters
function callTables(sel,clear){
    let content;
    if(!sel.selectedIndex){
        content = ATO;
    }
    else{
        content = ATP;
    }
    if(clear){
        let presetSel = document.querySelector('#preset');
        document.querySelector('#chosen').textContent='';
        presetSel.textContent ='';
        analyzeParams.length = 0;

        let key = 0;
        for(let keynum = 0; key = window.localStorage.key(keynum); keynum++){
            if(key.slice(-1) == sel.selectedIndex){
                let opt = document.createElement('option');
                opt.textContent = key.slice(0,-1);
                presetSel.appendChild(opt);
            }
        }
        if(presetSel.children.length != 0){
            document.querySelector('#RemovePreset').disabled = false;
        }
	document.querySelector('#paramInp').value = 'Enter Parameter Name';
    }
    let paramSel = document.querySelector('#sel');
    paramSel.textContent = '';
    let lines = content.split('\n');
    lines.forEach((line) => {
        if(!analyzeParams.includes(line)){
            let opt = document.createElement('option');
            opt.textContent = (line.split('\t'))[0];
            opt.value = (line.split('\t'))[1];
            paramSel.appendChild(opt);
        }
    });
    Object.values(document.querySelector('#parameter_div').children).forEach(function(child){
        child.removeAttribute('hidden');
    });
}

//Obtains parameter name & matches all corresponding parameters
//Parameter name is compatible with RegEx Expression Matching
function showResults(val){
    let res = document.querySelector('#sel');
    let ATindex = document.querySelector('#ATsel').selectedIndex;
    res.textContent='';
    autocompleteMatch(val,ATindex).forEach(line => {
            if(!analyzeParams.includes(line)){
                let opt = document.createElement('option');
                opt.textContent = (line.split('\t'))[0];
                opt.value = (line.split('\t'))[1];
                res.appendChild(opt);
            }
    });
}

function autocompleteMatch(input,ATindex) {
    if (input == '') {
        callTables(document.querySelector("#ATsel"),false);
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

//Transfer child element from curSel to receiveSel
function transferRows(curSel,receiveSel){
    let opt = curSel.options[curSel.selectedIndex];
    let analyze = document.querySelector('#analyze');

    if(receiveSel.id === "chosen"){
        analyzeParams.push(`${opt.textContent}\t${opt.value}`);
        analyzeParams.sort((a,b) => {
            let aVal = Number(a.split('\t')[1]);
            let bVal = Number(b.split('\t')[1]);
            if(aVal < bVal){
                return -1;
            }
            if(aVal > bVal){
                return 1;
            }
            return 0;
        })
        analyze.disabled = false;
    }
    else{
        analyzeParams.splice(curSel.selectedIndex,1);
        if(!analyzeParams.length){
            analyze.disabled = true;
        }
    }
    let paramInputValue = document.querySelector('#paramInp').value;
    if(paramInputValue==="Enter Parameter Name" || opt.textContent.match(paramInputValue)){
        receiveSel.insertChildAtIndex(opt);
    }
    else{
        curSel.removeChild(opt);
    }
}

Element.prototype.insertChildAtIndex = function(child) {
    this.appendChild(child);
    let indexToReplace = this.options.length;
    let sortedOptions = Object.keys(this.options).sort((a,b) => {
        if (Number(this.options[a].value) < Number(this.options[b].value)) {
            indexToReplace = b;
            return -1;    
        }    
        if (Number(this.options[b].value) < Number(this.options[a].value)) {
            return 1;    
        }    
        return 0;    
    });

    if(indexToReplace != this.options.length){
        for(let i = 0;i < sortedOptions.length-1; i++){
            if((Number(sortedOptions[i])+1)-Number(sortedOptions[i+1]) > 0){
                this.insertBefore(child,this.options[i]);
                return;
            }
        }
    }
}

//BASIC:
//Creation of Unique Presets
//Removal of Current Analysis To Correct Place
//ADVANCED:
//Full Overwriting 
//Append to current
//Duplicate from current and append
//Conflicting names between Log Types -- fix : add logtype to end of name so unique to type always
//Ask whether rename is necessary
function transferPresets(presetSel,chosenSel,paramSel){
    if(document.querySelector("#paramInp").value !== 'Enter Parameter Name'){
        callTables(document.querySelector("#ATsel"),false);
        document.querySelector("#paramInp").value = 'Enter Parameter Name';
    }

    let logType = document.querySelector("#ATsel").selectedIndex;
    let chosenPreset = presetSel.children[presetSel.selectedIndex];
    let presetParams = JSON.parse(window.localStorage.getItem(chosenPreset.textContent+logType)).params;
    for(let opt of Object.values(chosenSel.options)){
        paramSel.insertChildAtIndex(opt); 
    }
    analyzeParams = presetParams;
    for(let i = presetParams.length-1;i>-1;i--){
        let index = (presetParams[i].split('\t')[1])-1;
        let tempOpt = paramSel.children[index];
        chosenSel.insertChildAtIndex(tempOpt);
    }
}

function setPreset(removeButton,presetSel,val){
    let logType = document.querySelector("#ATsel").selectedIndex;
    window.localStorage.setItem(val+logType,JSON.stringify({logType: logType, params:[...analyzeParams]}));
    if(Object.values(presetSel.children).filter(child => child.textContent === val).length === 0){
        let opt = document.createElement('option');
        opt.textContent = val;
        presetSel.appendChild(opt);
        
        if(presetSel.children.length !== 0){
            removeButton.disabled = false;
        }
    }
}

function removePreset(removeButton,presetSel,val){
    let logType = document.querySelector("#ATsel").selectedIndex;
    window.localStorage.removeItem(val+logType);
    presetSel.removeChild(Object.values(presetSel.children).filter(child => {
        if(child.textContent === val){
            return child;
        }
    })[0]);

    if(presetSel.children.length === 0){
        removeButton.disabled = true;
    }
}

function analyze(analyzeButton){
    console.log("reached");
    analyzeButton.disabled = true;

    let blob = new Blob([makeConfig()], {type: "text/plain"});
    let dlink = document.createElement('a');
    dlink.download = "config.cmd";
    dlink.href = window.URL.createObjectURL(blob);
    dlink.onclick = function(e){
        console.log(e,this);
        let that = this;
        setTimeout(function(){
            window.URL.revokeObjectURL(blob);
        }, 1500);
    }
    dlink.click();
    dlink.remove();
}

function makeConfig(){
    let selectStart = document.querySelector("#start");
    let selectEnd = document.querySelector("#end");
    let contents = `set logType=\"${document.querySelector('#ATsel').selectedIndex}\"\nset files=\"`;
    for(const index of new Set(times.map(time => time.index))){
        contents = contents.concat(addedFiles[index].name).concat('\t');
    }
    contents = contents.slice(0,-1).concat('\"\n');
    contents = contents.concat(`set times=\"${selectStart.children[selectStart.selectedIndex].textContent.slice(-19).replace(/-/g,'/')}\t${selectEnd.children[selectEnd.selectedIndex].textContent.slice(-19).replace(/-/g,'/')}\"\n`);
    contents = contents.concat(`set params=\"${analyzeParams.map(str => str.split('\t')[1]).join("\t")}\"\n`);
    return contents;
}
