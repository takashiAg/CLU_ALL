// let packager = require('electron-packager');
// let rebuild = require('electron-rebuild');
//
// let dir = __dirname            // アプリ本体のフォルダ
// let out = __dirname + '/builds'       // 出力先のフォルダ
// console.log(dir, out)
// let option = {
//     dir,
//     out,
//     name: 'sample',            // アプリ名
//     arch: 'all',               // 32bit/64bit
//     platform: 'darwin,win32',         // OS
//     electronVersion: '1.7.13',  // Electronのバージョン
//     overwrite: true,           // すでにフォルダがある場合は上書き
//     prune: true,
//
//     afterCopy: [(buildPath, electronVersion, platform, arch, callback) => {
//         rebuild.rebuild({buildPath, electronVersion, arch, platform})
//             .then(() => callback())
//             .catch((error) => callback(error));
//     }],
//     // … other options
// }
//
// // function callback(e) {
// //     console.log(e)
// // }
//
// packager(option);
'use strict';

const builder = require('electron-builder');
const fs = require('fs');
const packagejson = JSON.parse(fs.readFileSync('./package.json', 'utf8'));

builder.build({
    platform: 'win',
    config: {
        'appId': `com.example.${packagejson.name}`,
        'win': {
            'target': 'zip',
        },
    },
});