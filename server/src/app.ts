import express, { Application } from 'express';
import swaggerUi from 'swagger-ui-express';
import morgan from 'morgan';
import router from './routes/routes';

const port = process.env.PORT || 3000;
const app: Application = express();

app.use(express.json());
app.use(morgan('dev'));
app.use(express.static('public'));

app.use('/api-docs', 
    swaggerUi.serve, 
    swaggerUi.setup(undefined, {
        swaggerOptions: {
            url: '/swagger.json'
        }, 
    })
);

app.use(router);


app.listen(port, () => {
    console.log(`Server listening on port ${port}`);
    console.log(`Swagger UI available at http://localhost:${port}/api-docs`);
});