/**
 * Created by 31641 on 2017-7-16.
 */
const Koa = require('koa');
const Io = require('socket.io')
const app = new Koa;
const io = new Io;

app.use(async function (ctx, next)
{
    let now = new Date();
    console.log(now.toTimeString());
    await next();
});

app.use(async function (ctx, next)
{
    ctx.body = "hello world";
    await next();
});

app.listen(3000);