const path = require('path');
const TerserPlugin = require('terser-webpack-plugin');

const root_path = path.resolve(__dirname, '../..');

const ts_loader = {
    loader: 'ts-loader',
    options: {configFile: path.resolve(__dirname, 'tsconfig.json')}
};

const node_loader = {
    loader: 'node-loader',
    options: {
        name: '[name].[ext]',
    },
};

const preprocessor_loader =
    'preprocessor-loader?line&file&config=' + path.join(__dirname, './preprocess.json');

function verboseConfig(from, to) {
    return {
        mode: 'production',  // development or production
        target: 'node',
        module: {
            rules: [
                {test: /\.tsx?$/, exclude: /node_modules/, use: [ts_loader, preprocessor_loader]},
                {test: /\.jsx?$/, exclude: /node_modules/, use: [preprocessor_loader]},
                {test: /\.node$/, use: [node_loader]},
            ],
        },
        resolve: {extensions: ['.ts', '.js', '.node'], alias: {'~': root_path}},
        context: root_path,
        optimization: {minimizer: [new TerserPlugin({extractComments: false})]},
        entry: from,
        output: {
            path: path.resolve(path.dirname(to)),
            filename: path.basename(to),
            libraryTarget: 'umd'
        },
    };
}

module.exports = [
    verboseConfig('./lib/addon.test.ts', './bin/addon.test.js'),
];
