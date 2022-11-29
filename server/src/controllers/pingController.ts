import { Get, Route } from 'tsoa';

interface IPingResponse {
    message: string;
}

@Route('ping')
export default class PingController {
    @Get('/')
    public async getPingMessage(): Promise<IPingResponse> {
        return {
            message: 'Pong!'
        };
    }
}