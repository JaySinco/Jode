import WebSocket from 'ws';
import axios from 'axios';
import {EventEmitter} from 'events';

declare const log: any;

export class NodeDebugger extends EventEmitter
{
    private requestId = 1;
    private ws?: WebSocket;
    private reqCallbackMap: Map<number, any> = new Map();
    private executionContextId = NaN;

    constructor()
    {
        super();
    }

    public connect(port: number): Promise<void>
    {
        return new Promise(async (resolve, reject) => {
            try {
                const response = await axios.get(`http://localhost:${port}/json/list`);
                const url = response.data[0].webSocketDebuggerUrl;
                this.ws = new WebSocket(url);
                this.ws.on('open', () => {
                    this.ws!.send(
                        JSON.stringify({id: this.getRequestId(), method: 'Debugger.enable'}));
                    this.ws!.send(
                        JSON.stringify({id: this.getRequestId(), method: 'Runtime.enable'}));
                });
                this.ws.on('message', (data) => {
                    const dataObj = JSON.parse(data.toString());
                    if (dataObj.method === 'Runtime.executionContextCreated' &&
                        isNaN(this.executionContextId)) {
                        this.executionContextId = dataObj.params.context.id;
                        resolve();
                        return;
                    }
                    if (dataObj.method === 'Runtime.executionContextDestroyed' &&
                        this.executionContextId === dataObj.params.executionContextId) {
                        this.emit('stopped');
                        return;
                    }
                    if (dataObj.method === 'Debugger.paused') {
                        this.emit('paused', dataObj);
                        return;
                    }
                    if (this.reqCallbackMap.has(dataObj.id)) {
                        this.reqCallbackMap.get(dataObj.id).resolve(dataObj);
                        this.reqCallbackMap.delete(dataObj.id);
                        return;
                    }
                });
                this.ws.on('error', (data) => {
                    log.error(`encounter websocket error: ${data}`);
                    this.close();
                });
                this.ws.on('close', () => {
                    log.debug('debugger client closed');
                    this.reqCallbackMap.forEach(v => v.reject('websocket closed'));
                    this.reqCallbackMap.clear();
                });
            } catch (e) {
                log.error(`failed to connect: ${e}`);
                reject(e);
            }
        });
    }

    public close(): void
    {
        this.ws!.close();
    }

    public runIfWaitingForDebugger(): Promise<any>
    {
        return this.callProtocol('Runtime.runIfWaitingForDebugger');
    }

    public setBreakpoint(url: string, lineNumber: number): Promise<any>
    {
        return this.callProtocol('Debugger.setBreakpointByUrl', {lineNumber: lineNumber - 1, url});
    }

    public removeBreakpoint(breakpointId: string): Promise<any>
    {
        return this.callProtocol('Debugger.removeBreakpoint', {breakpointId});
    }

    public pause(): Promise<any>
    {
        return this.callProtocol('Debugger.pause');
    }

    public resume(): Promise<any>
    {
        return this.callProtocol('Debugger.resume');
    }

    public evaluate(callFrameId: string, expression: string): Promise<any>
    {
        return this.callProtocol(
            'Debugger.evaluateOnCallFrame', {callFrameId, expression, returnByValue: true});
    }

    public waitTillPaused(): Promise<void>
    {
        return new Promise(resolve => {
            this.once('paused', dataObj => resolve(dataObj));
        });
    }

    public waitTillStopped(): Promise<void>
    {
        return new Promise(resolve => {
            this.once('stopped', () => resolve());
        });
    }

    public terminateExecution(): void
    {
        const id = this.getRequestId();
        this.ws!.send(JSON.stringify({id, method: 'Runtime.terminateExecution'}));
    }

    private callProtocol(method: string, params?: any): Promise<any>
    {
        return new Promise((resolve, reject) => {
            const id = this.getRequestId();
            this.ws!.send(JSON.stringify({id, method, params}));
            this.reqCallbackMap.set(id, {resolve, reject});
        });
    }

    private getRequestId(): number
    {
        return this.requestId++;
    }
}
