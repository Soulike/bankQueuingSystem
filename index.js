/**
 * Created by 31641 on 2017-7-16.
 */
const Koa = require('koa');
const IO = require('koa-socket-2');

const app = new Koa();
const io = new IO();

io.attach(app);

io.on('connection',async function (ctx, next)
{
    console.log(ctx.body);
    await next();
});

app.listen(3000);
