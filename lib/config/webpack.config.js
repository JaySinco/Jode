const path = require('path');
const TerserPlugin = require('terser-webpack-plugin');

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
        resolve: {extensions: ['.tsx', '.ts', '.js'], alias: {'~': path.resolve(__dirname, '..')}},
        context: path.resolve(__dirname, '../..'),
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
    verboseConfig('./lib/test_addon.js', './bin/test_addon.js'),
];
