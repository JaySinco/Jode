/// <reference types="node" />
export declare class shell {
    static moveItemToTrash(path: string): Promise<boolean>;
    static readClipboard(): [boolean, string];
    static writeClipboard(text: string): boolean;
}
