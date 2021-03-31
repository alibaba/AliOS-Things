const minify = require('minify');
const path = require('path');
const fs = require('fs');

const SOURCE_DIR = path.resolve('./doxygen/html');

const files = fs.readdirSync(SOURCE_DIR);

files.forEach((file) => {
  const fullPath = path.resolve(SOURCE_DIR, file);
  const stat = fs.lstatSync(fullPath);
  if (stat.isFile()) {
    const ext = path.extname(fullPath);
    if (ext === '.html' || ext === '.css' || ext === '.js') {
      minify(fullPath, {}).then((data) => {
        fs.writeFileSync(fullPath, data);
      }).catch(console.error);
    }
  }
});
