<?php

//!
//! ynk/frost/frost-meta.x86_64-apple-darwin.dev.php
//! author Maximilien M. Cura
//!

use \Frost\Ninja\{NinjaFile, NinjaRule, NinjaVar};
use \Frost\Annex\Invocation;
use \Frost\Env\PathSearcher;

return function(NinjaFile &$nf, Invocation $invocation) {
    base = $invocation->get('frost.cwd');

    $nf->decl('ninja_required_version', 1.8);
    $nf->decl('builddir', "{$base}/build");
    $nf->decl('root', "{$base}");

    $path_searcher = PathSearcher::from_env('PATH');
    $clang9 = $path_searcher->find_first('clang-9');
    $nf->decl('clang9', $clang9);

    $nf->decl('asflags', "--config {$base}/clang/x86_64-apple-darwin/dev-asflags.txt");
    $nf->decl('cflags', "--config {$base}/clang/x86_64-apple-darwin/dev-cflags.txt -I\$root");
    $nf->decl('cxxflags', "--config {$base}/clang/x86_64-apple-darwin/dev-cxxflags.txt -I\$root");
    $nf->decl('ldflags', "--config {$base}/clang/x86_64-apple-darwin/dev-ldflags.txt -L\$builddir");

    $nf->rule('rule-compile-cxx')
        -> command('${cache-engine} ${clang9} -MMD -MT $out -MF $out.d ${cxxflags} -c $in -o $out')
        -> describe('CXX $in')
        -> depend('$out.d', 'gcc');
    $nf->rule('rule-compile-c')
        -> command('${cache-engine} ${clang9} -MMD -MT $out -MF $out.d ${cflags} -c $in -o $out')
        -> describe('CC $in')
        -> depend('$out.d', 'gcc');
    $nf->rule('rule-compile-asm')
        -> command('${cache-engine} ${clang9} ${asflags} -c $in -o $out')
        -> describe('AS $in');
    $nf->rule('libdynamic')
        -> command('${clang9} -dynamiclib -fPIC -undefined dynamic_lookup -g -o ${ldflags} $out $in $libs')
        -> describe('LIBDYNAMIC $out');
    $nf->rule('link')
        -> command('${clang9} ${ldflags} -o $out $in $libs')
        -> describe('LINK $out');

    return;
}
