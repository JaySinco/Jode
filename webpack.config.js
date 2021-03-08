const path = require("path");
const TerserPlugin = require("terser-webpack-plugin");

module.exports = [
    {
        mode: "development",
        target: "node",
        entry: path.resolve(__dirname, "src", "res", "node-debugger.ts"),
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
        output: {
            path: path.resolve(__dirname, "bin"),
            filename: "node-debugger.js",
            libraryTarget: "umd"
        },
        context: path.resolve(__dirname),
        optimization: {
            minimizer: [
                new TerserPlugin({ extractComments: false })
            ]
        }
    }
];
