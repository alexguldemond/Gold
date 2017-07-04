let glob = require('glob');

process.argv.slice(2).forEach( (arg) => {
    glob(arg, (err, files) => {
	if (err) {
	    console.error(err);
	}
	files.forEach( (file) => {
	    console.log(file)
	});
    });
});
