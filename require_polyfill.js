function normalizeArray(parts, allowAboveRoot) {
  // if the path tries to go above the root, `up` ends up > 0
  var up = 0;
  for (var i = parts.length - 1; i >= 0; i--) {
    var last = parts[i];
    if (last === '.') {
      parts.splice(i, 1);
    } else if (last === '..') {
      parts.splice(i, 1);
      up++;
    } else if (up) {
      parts.splice(i, 1);
      up--;
    }
  }

  // if the path is allowed to go above the root, restore leading ..s
  if (allowAboveRoot) {
    for (; up--; up) {
      parts.unshift('..');
    }
  }

  return parts;
};

function pathNormalize(path) {
  var isAbsolute = path.charAt(0) === '/';
  var trailingSlash = path.substr(-1) === '/';

  // Normalize the path
  path = normalizeArray(path.split('/').filter(function(p) {
    return !!p;
  }), !isAbsolute).join('/');

  if (!path && !isAbsolute) {
    path = '.';
  }
  if (path && trailingSlash) {
    path += '/';
  }

  return (isAbsolute ? '/' : '') + path;
};

let current_dir_name = ".";
function is_relative(n) {
  if (n.length < 1) {
    return /* true */1;
  } else {
    return +(n[0] !== "/");
  }
}
var dir_sep = "/";

function is_dir_sep(s, i) {
  return +(s[i] === "/");
}
function concat(dirname, filename) {
  var l = dirname.length;
  if (l === 0 || is_dir_sep(dirname, l - 1 | 0)) {
    return dirname + filename;
  } else {
    return dirname + (dir_sep + filename);
  }
}

function custom_combine(path1, path2) {
  if (is_relative(path2)) {
    if (path2.length) {
      if (path1 === current_dir_name) {
        return path2;
      } else if (path2 === current_dir_name) {
        return path1;
      } else {
        return concat(path1, path2);
      }
    } else {
      return path1;
    }
  } else {
    return path2;
  }
}

var globalEval = eval;
var currentScript = document.currentScript;
var projectRoot = currentScript.dataset['project-root'] || currentScript.dataset['projectRoot'];
if (projectRoot == null) {
  throw new Error('The attribute `data-project-root` isn\'t found in the script tag. You need to provide the root (in which node_modules reside).')
}
var nodeModulesDir = projectRoot + '/node_modules/';

var modulesCache = {};
var packageJsonMainCache = {};

var ensureEndsWithJs = function(path) {
  if (path.endsWith('.js')) {
    return path;
  } else {
    return path + '.js';
  }
};
function loadScript(scriptPath) {
  var request = new XMLHttpRequest();

  request.open("GET", scriptPath, false); // sync
  request.send();
  var dirSeparatorIndex = scriptPath.lastIndexOf('/');
  var dir = dirSeparatorIndex === -1 ? '.' : scriptPath.slice(0, dirSeparatorIndex);

  var moduleText = `
(function(module, exports, modulesCache, packageJsonMainCache, nodeModulesDir) {
  function require(path) {
    var __dirname = "${dir}/";
    var resolvedPath;
    if (path.startsWith('.')) {
      // require('./foo/bar')
      resolvedPath = ensureEndsWithJs(custom_combine(__dirname, path));
    } else if (path.indexOf('/') === -1) {
      // require('react')
      var packageJson = pathNormalize(custom_combine(nodeModulesDir, custom_combine(path, 'package.json')));
      if (packageJsonMainCache[packageJson] == null) {
        var jsonRequest = new XMLHttpRequest();
        jsonRequest.open("GET", packageJson, false);
        jsonRequest.send();
        var main;
        if (jsonRequest.responseText != null) {
          main = JSON.parse(jsonRequest.responseText).main;
        };
        if (main == null) {
          main = 'index.js';
        } else if (!main.endsWith('.js')) {
          main = main + '.js';
        }
        packageJsonMainCache[packageJson] = custom_combine(nodeModulesDir, custom_combine(path, main));
      }
      resolvedPath = packageJsonMainCache[packageJson];
    } else {
      // require('react/bar')
      resolvedPath = ensureEndsWithJs(custom_combine(nodeModulesDir, path));
    };
    resolvedPath = pathNormalize(resolvedPath);
    if (modulesCache[resolvedPath] != null) {
      return modulesCache[resolvedPath];
    };
    var result = loadScript(resolvedPath);
    modulesCache[resolvedPath] = result;
    return result;
  };
  var process = {env: {}, argv: []};
  var global = {};


// -------Begin Require Polyfilled Module Loaded From Disk------------------------------
// file: ${scriptPath}
// root: ${projectRoot}
// ----------------------------------------------------------------------
${request.responseText}
// -------End Polyfill Loaded From Disk------------------------------
// file: ${scriptPath}
// root: ${projectRoot}
// ----------------------------------------------------------------------
return module.exports})\n//@ sourceURL=${scriptPath}`;
  var module = {exports: {}};
  return globalEval(moduleText)(module, module.exports, modulesCache, packageJsonMainCache, nodeModulesDir);
};

loadScript(currentScript.dataset.main)
