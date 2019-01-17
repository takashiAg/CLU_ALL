const fs = require('fs');
const remote = require('electron').remote;
const Dialog = remote.dialog;

$(function () {
    var file_open = document.getElementById('file_open');
    file_open.addEventListener('change', handleFileSelect, false);
    $("#save").on("click", function () {
        save_file();
    })
    //$("#test").on("click", function () {
    //    handleDownload();
    //})
});

function handleFileSelect(evt) {
    var files = evt.target.files[0];
    read_file(files);
}

function read_file(file) {
    var file_reader = new FileReader();
    file_reader.addEventListener('load', function () {
        var file_string = file_reader.result;
        var split_data = split_csv(file_string);
        make_table_from_data(split_data);
    }, false);
    file_reader.readAsText(file);
}

function split_csv(data) {
    var return_data = [];
    data.split("\n").forEach(function (value, index) {
        return_data.push(value.split(","));
    });
    return return_data;
}

function clear_box(target) {
    $(target).empty();
}

function make_table_from_data(data) {
    clear_box("#comport")
    data.forEach(function (value) {
        if (value.length == 6) {
            row = "<tr>";
            value.forEach(function (column) {
                row += "<td><input class=\"clear_box\" value=\"" + column + "\"></td>";
            });
            row += "<td class=\"delete_colmn\" ><span class=\"glyphicon glyphicon-minus\"></span></td>";
            row += "</tr>";
            $("#comport").append(row);

            $(".delete_colmn").off("click");
            $(".delete_colmn").on("click", function () {
                $(this).parent().remove();
            })
        }
    });
}


function make_data_from_table() {

    var table = [];

    $("#comport tr").each(function (index, element) {
        var row = [];
        $(this).children().each(function () {
            row.push($(this).find("input").val());
        });
        table.push(row);
    });
    return table;

}

function save_file() {
    var data = make_data_from_table();
    var converted_data = convert_data_to_file(data);
    setTimeout(function () {
        Dialog.showSaveDialog({
            title: '保存',
            defaultPath: '.',
            filters: [
                {name: 'SP35構成機用パラメータファイル', extensions: ['SP35']}
            ]
        }, (savedFiles) => {
            writeFile(savedFiles, converted_data);
            console.log(savedFiles);
        });
    }, 1)

    console.log(converted_data);
}


function convert_data_to_file(data) {
    var return_data = "";
    data.forEach(function (value) {
        return_data += value[0] + ",";
        return_data += value[1] + ",";
        return_data += value[2] + ",";
        return_data += value[3] + ",";
        return_data += value[4] + ",";
        return_data += value[5] + "\n";
    });
    return return_data;
}

function writeFile(path, data) {
    fs.writeFile(path, data, 'utf8', function (err) {
        if (err) {
            return console.log(err);
        }
    });
}