import {Body, Get, Post, Route, ValidateError, BodyProp} from 'tsoa';
import {PrismaClient} from '@prisma/client';

interface IinitResponse {
    data: {
        title: string,
        content: string,
        published: boolean,
    }
}

interface IRepository {
    id: number,
    name: string,
}

const prisma = new PrismaClient();


@Route('repository')
export default class RepositoryController {
    @Get('init/{name}')
    public async getInitName(name: string){
        return await prisma.repository.findMany({
            where: {
                name: name,
            },
        });
    }

    @Post()
    // Compare this snippet from src/controllers/repositoryController.ts:
    public async createRepository(req: any){
        console.debug(`The repository name is: ${req.params.name}`);
        return result = await prisma.repository.create({
            data: {
                id: 1,
                name: req.name,
            },
        });
    }

    @Post('add/{file}')
    public async addFile(req: any, res: any) {
        const newLocal = "hello";
        const result = await prisma.repository.create({
            data: {
                id: 1,
                name: req.params.name
            },
        });
        return res.json(result);
    }

    @Post('commit/{message}')
    public async commit(req: any, res: any) {
        const result = await prisma.commit.create({
            data: {
                id: 1,
                name: req.params.name
            },
        });
        return res.json(result);
    }

}
