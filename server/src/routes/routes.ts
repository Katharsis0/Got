import RepositoryController from '../controllers/repositoryController';
import PingController from '../controllers/pingController';
import express from 'express';

const router = express.Router();

// TODO
//got init <name>:
//
// got add [-A] [name]:
// got commit <mensaje>:
// got status <file>:
// got rollback <file> <commit>
// got reset <file>
// got sync <file>

router.get('/ping', async (req, res) => {
    const controller = new PingController();
    const response = await controller.getPingMessage();
    return res.send(response);
});

router.get('/user/:user_name', async (req, res) => {
  const controller = new RepositoryController();

});

router.post('/repository/init/:name', async (req, res) => {
  const controller = new RepositoryController(); 
  const { name, owner } = req.body;
  console.debug(`The repository name is: ${name}`);
  const result = await controller.createRepository(req);
  return res.send(result);
});

router.put('/repository/add/:file', async( req, res ) => {
  const controller = new RepositoryController();
  const { file } = req.body;
  const result = await controller.addFile(req, res);
  return res.send(result);
});

router.post('/repository/commit/:message', async (req, res) => {

});

router.get('/repository/status/:repository_name', async( req, res ) => {

});

router.get('/repository/rollback/:file/:commit_hash', async( req, res ) => {

});

router.get('/repository/sync/:file', async(req, res) => {

});

router.delete('/repository/:name', async(req, res) => {


});

export default router;