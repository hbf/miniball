#!/usr/local/bin/perl

#  gensuit.pl
#
#  Martin Kutz
#  Kaspar Fischer
#
#  Mar. 2003


print "========================================\n";
print "Test Suit Generator\n";
print "========================================\n";


# parse command line
($directory,$method,$nlist,$dlist) = @ARGV;

die "usage: $0 directory method numbers dimensions\n"
    unless $directory and $method and $nlist and $dlist;

# mkdir
$directory .= "/" unless $directory =~ /\/$/;
die "Error: cannot create directory $directory\n"
    unless -d $directory or mkdir $directory, oct 777;

@N = split /\,/, $nlist;
@D = split /\,/, $dlist;

$" = ", ";
print "n = @N\n";
print "d = @D\n";
print "method: $method\n";
print "directory: $directory\n";

# underscore method
$_method = $method;
$_method =~ s/\s+/_/g;

for $d (@D) {
    for $n (@N) {
	$file = $directory.$_method."_".$n."_".$d.".pts";
	print "creating $file.gz\n";
	open (GEN,"./gen_random $n $d $method |")
	    or die "Failed.\n";
	open (OUT,">$file")
	    or die "Failed.\n";
	while (<GEN>) {
	    die $_ if /error/i;
	    print OUT;
	}
	close GEN;
	close OUT;

	system (gzip,"-f",$file)
	    and die "Error: zipping failed\n";
    }
}
