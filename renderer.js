// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.

const serialPort = require('serialport');
var SP;
var response = "";
var serial_histry = [];

window.onload = function () {
    //setTimeout(make_serial_list(),1);
    make_serial_list();

    bind_button();


}

function bind_button() {
    $("#reload_serial").on("click", function () {
        make_serial_list()
    })
    $("#message_send").on("click", function () {
        write();
    })
    $("#add_row").on("click", function () {
        add_row();
    })
    $(".delete_colmn").on("click", function () {
        $(this).parent().remove();
    })
    $(".close").each(function () {
        $(this).on("click", function () {
            $("#exampleModalCenter").css("display", "none").removeClass("in")
        })
    })
}

function make_serial_list() {
    console.log("make_serial_list")
    var port_names = []
    serialPort.list(function (err, ports) {
        ports.forEach(function (port) {
            port_names.push({"comName": port.comName, "manufacturer": port.manufacturer})
        });
        setTimeout(function () {
            serial_table(port_names)
        }, 1);
    });

    //
}


function serial_table(ports) {
    console.log(ports)
    var flag = true;
    $("#comports").empty()
    ports.forEach(function (port) {
        flag = false;
        var com = "<tr>" +
            "<th scope=\"row\"><a href='#' class='comport'>" + port.comName + "</a></th>" +
            "<td>" + port.manufacturer + "</td>" +
            "</tr>";
        $("#comports").append(com);
    });
    if (flag) {
        var com = "<tr>\n" +
            "<th scope=\"row\">NO device detected</th>\n" +
            "<td>NO device detected</td>\n" +
            "</tr>";
        $("#comports").append(com);
    }
    setTimeout(make_event(), 1);
}


function make_event() {
    $(".comport").off()
    $(".comport").on("click", function () {
        start_serial($(this).text())
    });
}

function start_serial(serial_port) {
    SP = new serialPort(serial_port, {
        baudRate: 115200,
        parser: new serialPort.parsers.Readline("\n"),
    });
    SP.on('open', function () {
        serial_open()
    });
    SP.on('data', function (res) {
        recieved_message(res)
    });
    SP.on('error', function (err) {
        serial_err(err)
    });
    SP.on('close', function (err) {
        serial_close(err)
    })
}

function send_message(message) {
    SP.write(message, function (err, results) {
        //console.log('err ' + err);
        //console.log('results ' + results);
    });
}

function recieved_message(res) {
    console.log('data received: ' + res);

    response += res;
    var Lines = response.split("\n")
    var Line_length = Lines.length;
    response = Lines[Line_length - 1];
    Lines.pop();
    if (Line_length > 1) {
        Lines.forEach(function (element) {
            //console.log(element.length, element);
            var param_last_sign = "that was params"
            //console.log(param_last_sign.length, param_last_sign);
            if (element == param_last_sign) {
                var parameter = serial_histry[serial_histry.length - 1]
                console.log(parameter)
                var parameter_array = eval(parameter);
                $("#device").empty();
                parameter_array.forEach(function (element) {
                    if (!(element[0] == "0" && element[1] == "0" && element[2] == "0" && element[3] == "0" && element[4] == "0" && element[5] == "0")) {
                        $("#device").append(
                            "<tr>" +
                            "<td>" + element[0] + "</td>" +
                            "<td>" + element[1] + "</td>" +
                            "<td>" + element[2] + "</td>" +
                            "<td>" + element[3] + "</td>" +
                            "<td>" + element[4] + "</td>" +
                            "<td>" + element[5] + "</td>" +
                            "</tr>");
                    }
                });
                setTimeout(function () {
                    $("#exampleModalCenter").css("display", "block").addClass("in")
                }, 1)
                serial_histry = []
            }
            serial_histry.push(element);
        })
    }
    //console.log(serial_histry);

    /*
    var dt = new Date()
    var com = "<tr>\n" +
        "<th scope=\"row\">" + dt.toString() + "</th>\n" +
        "<td>" + res + "</td>\n" +
        "</tr>";
    $("#comport").append(com);
    */
}

function serial_open() {
    //console.log("serial on");
    $("#status_bar").removeClass("btn-primary");
    $("#status_bar").addClass("btn-success");
    $("#status").text("接続されました");
}

function serial_err(err) {
    //console.log(err)
}

function serial_close(err) {
    //console.log('port closed', err);
    $("#status_bar").removeClass("btn-success");
    $("#status_bar").addClass("btn-primary");
    $("#status").text("切断されました");
}

function write() {

    var data = make_data_from_table();

    var message = decode_serial(data);

    //send_message(message);
    //multi_send_message(message);
    send_data_to_device(message);

}

function send_data_to_device(messages) {
    var length = messages.length;
    var counter = 0;

    //console.log(messages)
    /* 一定時間ごとに実行する関数
     * node_serialがうまく実行できない時があるので。
     */
    var id = setInterval(function () {
        if (counter < length) {

            console.log(messages[counter]);

            //デバイスに送信
            send_message("start" + messages[counter++] + "\n\n");

        } else {

            //endflagを送信
            send_message("endend");

            //alert("デバイスに設定情報を送信しました。")

            //setInterval()を解除する。
            clearInterval(id);
        }
    }, 100);
}

function multi_send_message(messages) {
    messages.forEach(function (message) {
        send_message("start" + message + "\n");
    });
    //endflagを送信
    send_message("endend");
}

function int_to_2byte_hex_string(input_data) {
    var return_value = ""
    if (input_data >= 256 * 16) {
        return_value = input_data.toString(16);
    } else if (input_data >= 256) {
        return_value = "0" + input_data.toString(16);
    } else if (input_data >= 16) {
        return_value = "00" + input_data.toString(16);
    } else if (input_data > 0) {
        return_value = "000" + input_data.toString(16);
    } else {
        return_value = "0000";
    }
    return return_value;
}

function decode_serial(data) {

    var return_value = new Array();
    data.forEach(function (value) {
        var column = "";
        for (var i = 0; i < 6; i++) {
            column += "" + int_to_2byte_hex_string(value[i] - 0) + "";
        }
        return_value.push(column);
        //console.log(column);
    });
    return return_value;
    /*
    var message = "";
    data.forEach(function (value, index) {
        var message_row = "" +
            value[0] + "," +
            value[1] + "," +
            value[2] + "," +
            value[3] + "," +
            value[4] + "," +
            value[5] + ";";
        message += message_row;
    });
    return message + "*";
    */
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

function add_row() {
    console.log("added row");
    row = "<tr>" +
        "<td><input class=\"clear_box\"></td>" +
        "<td><input class=\"clear_box\"></td>" +
        "<td><input class=\"clear_box\"></td>" +
        "<td><input class=\"clear_box\"></td>" +
        "<td><input class=\"clear_box\"></td>" +
        "<td><input class=\"clear_box\"></td>" +
        "<td class=\"delete_colmn\" ><span class=\"glyphicon glyphicon-minus\"></span></td>" +
        "</tr>"
    $("#comport").append(row);
    $(".delete_colmn").off("click");
    $(".delete_colmn").on("click", function () {
        $(this).parent().remove();
    })
}