const path = require("path");
const TerserPlugin = require("terser-webpack-plugin");

function verboseConfig(from, to) {
    return {
        mode: "development",
        target: "node",
        module: {
            rules: [
                {
                    test: /\.tsx?$/,
                    exclude: /node_modules/,
                    use: [
                        "ts-loader",
                        "preprocessor-loader?line&file&config=" + path.join(__dirname, "./preprocess.json"),
                    ]
                },
                {
                    test: /\.node$/,
                    loader: "node-loader",
                },
            ],
        },
        resolve: {
            extensions: ['.tsx', '.ts', '.js'],
            alias: {
                "~": path.resolve(__dirname)
            }
        },
        context: path.resolve(__dirname),
        optimization: {
            minimizer: [
                new TerserPlugin({ extractComments: false })
            ]
        },
        entry: path.resolve(from),
        output: {
            path: path.resolve(path.dirname(to)),
            filename: path.basename(to),
            libraryTarget: "umd"
        },
    };
}

module.exports = [
    verboseConfig('./src/res/test.ts', './bin/test.js'),
];
