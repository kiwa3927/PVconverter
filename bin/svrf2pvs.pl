#!/usr/bin/perl -w
use strict;
use warnings;

use Getopt::Long;
my %Options=();
GetOptions (\%Options,
		"check"
	  );
#
# Filtering rule file
#

print "\
**************************************************************************************\
***                              SVRF2PVS LICENSE                                 ****\
**************************************************************************************\
\
Redistribution and use in source and binary forms, with or without\
modification, are permitted provided that the following conditions are met:\
\
Redistributions of source code must retain the above copyright notice, this \
list of conditions and the following disclaimer. \
\
Redistributions in binary form must reproduce the above copyright notice, \
this list of conditions and the following disclaimer in the documentation \
and/or other materials provided with the distribution. \
\
Neither the name of the copyright holder nor the names of its \
contributors may be used to endorse or promote products derived from \
this software without specific prior written permission. \
\
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" \
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE \
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE \
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE \
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR \
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF \
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS \
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN \
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) \
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE \
POSSIBILITY OF SUCH DAMAGE. \
\
************************************************************************************** \n\
";

my $usage = "Usage: $0 inFile\n";
my %rules;

#@ARGV = ("test.drc");

(@ARGV == 1) or die $usage;
my $gf = $ARGV[0];
my @fds = ();
open(GIN, $gf) or die "Error: can not open $gf\n";
push(@fds, \*GIN);

my $level = 0;
my @passes = ();
$passes[$level] = 1;

my %defs = ();
my $ruleskip = 0;
my $buf = "";
my $spline = 0;
my %parseFlag = ();
my $haveTvfCode = 0;
my $isTvf = 0;
my $bpLevel = 0;
&initHashFlag(\%parseFlag);

my @lineBuffer = ();
my @flagBuffer = ();
my @commentBuffer = ();
my $ruleComment = "";
my @readBuffer = ();
my %lastCmd = ();
my $openSquareBracket = 0;
my $haveComplexCmd = 0; # complex command could have empty line inside
while (@fds) {
	my $fin = pop(@fds);
	my $line;
	while ($line = <$fin>) {
		# s#/\*.*?\*/##sg;
		if ($line =~ /^\s*\/\//) {
			&appendComment($line);
			#print "$line";
			next;
		}
		if ($isTvf == 1 and $parseFlag{inTvf} == 1) {
			if ($line =~ /^\s*#/) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*set/is) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*exit/is) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*puts\s+/is) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*{.*}\s*/) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s+$/) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*foreach/) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*for/) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*namespace\s+/) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*;\/\/.*/) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*}\s*$/) {
				&outAllCommand();
				print "$line";
				next;
			} elsif ($line =~ /^\s*tvf::setlayer\s+\S+/is) {
				&outAllCommand();
				my ($keys, $inLine) = ($line =~ /^\s*(tvf::setlayer)\s+(.*)$/is);
				my ($transRes, $err) = &translateRule($line,	\%parseFlag);
				&formatLine(\$transRes);
				$transRes =~ s/;\s*$//;
				if ($err == 0) {
					print "tvf::SETLAYER $transRes\n";
					#print "VRF::LAYERDEF $transRes\n";
				}
				next;
			} elsif ($line =~ /^\s*tvf::rulecheck\s+\S+/is) {
				&outAllCommand();
				my ($tab, $rule) = ($line =~ /^(\s*)tvf::rulecheck\s+(\S+)/is);
				print "$tab"."tvf::RULECHECK $rule {\n";
				#print "$tab"."VRF::RULE $rule {\n";
				next;
			} elsif ($line =~ /^\s*tvf::@/is) {
				&outAllCommand();
				my ($tab, $comment) = ($line =~ /^(\s*)tvf::@(.*)/is);
				print "$tab"."tvf::@"."$comment\n";
				#print "$tab"."VRF::@"."$comment\n";
				next;
			} elsif ($line =~ /^\s*tvf::outLayer/is) {
				&outAllCommand();
				my ($tab, $statement) = ($line =~ /^(\s*)tvf::outLayer\s*(.*)/is);
				$statement =~ s/^\s*"//;
				$statement =~ s/"\s*$//;
				my ($res, $isStack, $err) = ("", 0, 0);
				($res, $isStack, $err, $statement) = 
						&transGenericStatement($statement, $fin, 
							0, \%parseFlag);
				if ($err == 0) {
					$res =~ s/\s*;\s*$//is;
					print "$tab"."tvf::OUTLAYER"." \"$res\"\n";
					#print "$tab"."VRF::LAYER_OUT"." \"$res\"\n";
				} else {
					print "$tab"."tvf::OUTLAYER"."!!!CHK_TV!!!<<$statement>>\n";
					#print "$tab"."VRF::LAYER_OUT"." \"$res\"\n";
				}
				next;
			} elsif ($line =~ /^\s*if\s*{/) {
				&outAllCommand();
				print $line;
				next;
			} elsif ($line =~ /^\s*tvf::VERBATIM\s+{$/) {
				&outAllCommand();
				my $outLine = "tvf::VERBATIM {\n";
				#my $outLine = "VRF::RULE_BLOCK {\n";
				$parseFlag{inTvf} = 0;
				print $outLine;
				next;
			} elsif ($line =~ /^\s*VERBATIM\s+{$/) {
				&outAllCommand();
				my $outLine = "VERBATIM {\n";
				$parseFlag{inTvf} = 0;
				print $outLine;
				next;
			}
				
				
				
		}
		if ($line =~ /^\s*@/ and $passes[$level] != 0) {
			&outAllCommand();
			print "$line";
			next;
		}
		if ($line =~ m#\/\*# and $line !~ /^\s*tvf\s+function\s+\S+/is) {
			&outAllCommand();
			my $commline = "";
			my $inCommLine = $line;
			while(1) {
				$commline .= $inCommLine;
				last if ($inCommLine =~ m#\*\/#);
				$inCommLine = <$fin>;
			}
			$line = $commline;
			print $commline;
			$line =~ s#/\*.*\*/##sg;
		}
		$line =~ s#\s*//.*##;

		#$line =~ s#{\s*@.*#{#;
		if ($line =~ /\S+\s*{\s*@.*$/) {
			($line, $ruleComment) = ($line =~ /^(.*?{)\s*(@.*)$/);
		}


		if ($line =~ /^\s*include\s+(\S+)/is) {
			push(@fds, $fin);
			my $newFile;
			newopen(\$newFile, $1);
			$fin = $newFile;
			next;
		}
		if ($isTvf == 1 and $parseFlag{inTvf} == 1 and $line =~ /^\s*source\s+(\S+)/is) {
			push(@fds, $fin);
			my $newFile;
			newopen(\$newFile, $1);
			$fin = $newFile;
			&outAllCommand();
			print "// SOURCE FILE >> $line";
			next;
		}
		if ($line =~ /^\s*#ifdef\s+(\S+)\s*$/is) {
			++$level;
			$passes[$level] = checkDefined($1) ? $passes[$level-1] : 0;
			next;
		}
		if ($line =~ /^\s*#ifdef\s+(\S+)\s+(\S+)/is) {
			++$level;
			$passes[$level] = checkDefinedValue($1, $2) ?
						$passes[$level-1] : 0;
			next;
		}
		if ($line =~ /^\s*#ifndef\s+(\S+)\s*$/is) {
			++$level;
			$passes[$level] = checkDefined($1) ? 0: $passes[$level-1];
			next;
		}
		if ($line =~ /^\s*#ifndef\s+(\S+)\s+(\S+)/is) {
			++$level;
			$passes[$level] = checkDefinedValue($1, $2) ?
						0 : $passes[$level-1];
			next;
		}
		if ($line =~ /^\s*#else/is) {
			$passes[$level] =
				($passes[$level-1]==1 && $passes[$level]==0) ? 1 : 0;
			next;
		}
		if ($line =~ /^\s*#endif/is) {
			--$level;
			next;
		}
		if ($line =~ /^#DECRYPT /) {
			&outAllCommand();
			my $cryptLine = "";
			while ($cryptLine = <$fin>) {
				#print $cryptLine;
				last if ($cryptLine =~ /^#ENDCRYPT/);
			}
			next;
		}
		next if ($passes[$level] != 1);

		if ($line =~ /^\s*#define\s+(\S+)\s*$/is) {
			$defs{$1}=defined;
			next;
		}
		if ($line =~ /^\s*#define\s+(\S+)\s+(\S+)/is) {
			$defs{$1}=$2;
			next;
		}
		if ($line =~ /^\s*#undefine\s+(\S+)/is) {
			$defs{$1}=undef;
			next;
		}
		if ($line =~ /^#DECRYPT /) {
			&outAllCommand();
			my $cryptLine = "";
			while ($cryptLine = <$fin>) {
				#print $cryptLine;
				last if ($cryptLine =~ /^#ENDCRYPT/);
			}
			next;
		}
		if ($line =~ /^#ENCRYPT\s*$/) {
			&outAllCommand();
			print "$line";
			next;
		}
		if ($line =~ /^#ENDCRYPT/) {
			&outAllCommand();
			print "$line";
			next;
		}
			
		my $isExpression = 0;
		if ($line =~ /^\s*\[.*\]/is) {
			$isExpression = 1;
		} elsif ($line =~ /\blitho\s+file.*\[/is) {
			my $newLine = $line;
			while ($newLine !~ /\]/) {
				$newLine = <$fin>;
				$line .= $newLine;
				if ($newLine =~ /\]/) {
					last;
				}
			}
			$isExpression = 1;
		}
		if ($bpLevel == 0 and $line =~ /^\s*[^\t \{\}\n]+\s*$/ and $line !~ /^\s*disconnect\s*$/is and $line !~ /\([^)]*$/) {
			if ($ruleskip != 1) {
				if ($line =~ /^#!\s*tvf/i) {
					#print "#!VRF_tcl\n";
					print "#!tvf\n";
					$isTvf = 1;
					$parseFlag{inTvf} = 1;
					next;
				} else { 
					if ($line =~ /^\s*\[.*\]/is) {
						$isExpression = 1;
					} elsif ($line =~ /^\s*\[/is) {
						my $unfinishedBastket = 1;
						while ($unfinishedBastket != 0) {
							my $newLine = <$fin>;
							chomp $line;
							$line .= $newLine;
							if ($newLine =~ /\]/is) {
								$unfinishedBastket = 0;
							}
						}
						$isExpression = 1;
					} else {
						print "$line>", $buf, "\n" if ($buf ne "");
						#$buf = $1;
						#if ($line =~ /^\s*\[\s*$/is) {
						my $flag = &saveHashFlag(\%parseFlag);
						if (&isCommandInParenthesis($line)) {
							&addNewGoodCommand($line, $flag); 
						} else {
							&appendBadCommand($line, $flag); 
						} 
						next;
					}
				}
			}
		} elsif ($line =~ /^\s*\{/is) {
			my $last = &takeLastWord;
			my $flag = &saveHashFlag(\%parseFlag);
			if ($last =~ /\S+/) {
				$line = "$last $line";
				&addNewGoodCommand($line, $flag);
			} else {
				&appendBadCommand($line, $flag); 
			}
			next;
		} else {
			if (not defined($buf)) {
				print "!!CHK_UNDEF>>$buf<< when parsing !!INLINE_BEGIN<<$line>>INLINE_END!!\n";	
			}
			print "??$line>", $buf, "\n" if ($buf ne "");
			$buf = "";
		}

#		if (scalar(%rules) != 0) {
#			$ruleskip = 0;
#		} else {
#			if (exist ($rules{$1})) {
#				$ruleskip = 1;
#			} else {	
#				$ruleskip = 0;
#			}
#		}
		if ($line =~ /^\s*$/) {
			++$spline;
			next if ($spline > 1);
			$line = "\n";
		} else {
			$spline = 0;
		}
		if ($line =~ /^\s*$/ and $bpLevel == 0) {
			if ($haveComplexCmd == 1) {
				my $flag = &saveHashFlag(\%parseFlag);
				&appendBadCommand($line, $flag);
			} else {
				&outAllCommand();
				print $line;
			}
			next;
		}
		if ($parseFlag{inRule} == 0 && $line =~ /^\s*(\S+)\s*{\s*\S+.*$/is) {
			&outAllCommand();
			my ($l1, $l2) = ($line =~ /^([^{]+{)(.*)$/is);
			my ($res1, $err) = &translateRule($l1, \%parseFlag);
			if ($err == 0) {
				&printLine($res1, \%parseFlag);
			}
			$line = $l2;
		}	
		
		if ($parseFlag{inRule} == 1 && $line =~ /^\s*\S+.*\s*}/is) {
			&outAllCommand();
			my ($l1, $l2) = ($line =~ /^([^}]+)}(.*)$/is);
			my ($res1, $isStack, $err, $tmpLine) = &transGenericStatement($l1, $fin, 
														$isExpression, \%parseFlag);
			#&translateRule($l1, \%parseFlag);
			if ($err == 0) {
				&printLine($res1, \%parseFlag);
			}
			my ($res2, $err2) = &translateRule("}\n", \%parseFlag);
			if ($err2 == 0) {
				&printLine($res2, \%parseFlag);
			}
			if ($l2 =~ /\S+/) {
				my ($res3, $err3) = &translateRule($l2, \%parseFlag);
				if ($err3 == 0) {
					&printLine($res3, \%parseFlag);
				}
			}
			next;
		}
		my $closeCurlyParenthesis = 0;
		if ($parseFlag{inRule} == 1 && $line =~ /^\s*}s*$/) {
			$closeCurlyParenthesis = 1;
		}
		if (&isFuncDefBegin($line)) {
			$line = &loadFullFunction($line, $fin);
		}
		my $flag = &saveHashFlag(\%parseFlag);
		my ($res, $isStack, $err) = ("", 0, 0);
		my $haveSquare = 0;	
		if (&isCmdNeedSquareExp(\%lastCmd) or $bpLevel > 0) {
			if ($bpLevel == 0) {
				($haveSquare, $openSquareBracket) = &isSquareBracket($line, $openSquareBracket);
			}
			if (($bpLevel > 0) or ($haveSquare == 1 and $openSquareBracket == 1)) {
				&finishSquareExp(\%lastCmd, $fin, $line, $isExpression, \%parseFlag);
				$haveSquare = 0;
				$openSquareBracket = 0;
				if ($bpLevel == 0) {
					#print "Reach here\n";
					&appendExpr($lastCmd{expr});
					$lastCmd{expr} = "\n";
				}
				next;
			}
		} 
		($res, $isStack, $err, $line) = &transGenericStatement($line, $fin, 
									$isExpression, \%parseFlag);
									
		my $isAssign = &isAssignment($line);
		if ($isAssign == 0) {
			($haveSquare, $openSquareBracket) = &isSquareBracket($line, $openSquareBracket);
		}
		if ($err == 1 and ($isAssign == 0 or $haveSquare == 1)) {
			&appendBadCommand($line, $flag);
		} elsif ($closeCurlyParenthesis == 1) {
			&addNewGoodCommand($line, $flag);
			&outAllCommand();
			$haveComplexCmd = 0;
		} else {
			if (&isComplexCmd($res)) {
				$haveComplexCmd = 1;
			} elsif ($err == 0) {	
				$haveComplexCmd = 0;
			}
			if ($isStack == 1) {
				&outAllCommand();
				#&printLine($res, \%parseFlag);
				&addNewGoodCommand($line, $flag);
			} else {
				&addNewGoodCommand($line, $flag);
			}
		}
			
	}
	&outAllCommand();
	print $buf, "\n" if ($buf ne "");
	close $fin;
}

sub newopen {
	my ($refFile, $path) = @_;
	$path =~ s/^\s*"//;
	$path =~ s/"\s*$//;
#	local *FH;  # not my!
	if (not -r $path) {
		die "###FATAL### could not open file $path for read\n";
	}
	open $$refFile, $path 
		or $$refFile = undef;
}

sub checkDefined {
	my $var = shift;
	return 1 if (defined $defs{$var});
	return ($var =~ s/^\$//) ? (defined $ENV{$var}) : 0;
}

sub checkDefinedValue {
	my $var = shift;
	my $value = shift;
	return 1 if ((exists $defs{$var}) and $defs{$var} eq $value);
	return ($var =~ s/^\$//) ? (exists $ENV{$var} and $ENV{$var} eq $value) : 0;
}

sub translateRule {
	my ($inStatement, $refFlag) = @_;

	my $isExpr = &checkIsExpress($inStatement);
	if ($isExpr == 1) {
		return ($inStatement, 1);
	}
	my $inLine = &replaceKeywords($inStatement);
	my $outLine = $inLine;

	my $incomplete = 0;
	#$refFlag->{incomplete} = 0;
	$refFlag->{inRuleCap} = 0;
	if ($inLine =~ /^\s*$/) {
		$outLine = "\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^#!\s*tvf/i) {
		$haveTvfCode = 1;
		#$outLine = "#!VRF_tcl\n";
		$outLine = "$inLine\n";
		$refFlag->{inTvf} = 1;
		$isTvf = 1;
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*checkname\s+/i) {
		return ($inLine, 1);
	} elsif ($inLine =~ /^\s*comment\s+/i) {
		return ($inLine, 1);
	} elsif ($inLine =~ /^\s*layout\s+bump2/is) {
		my ($fileName) = ($inLine =~ /^\s*layout\s+bump2\s+(\S+)/is);
		$fileName =~ s/^"//;
		$fileName =~ s/"$//;
		$outLine = "bump2 \"$fileName\";\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+path2/is) {
		my ($fileName) = ($inLine =~ /^\s*layout\s+path2\s+(\S+)/is);
		$fileName =~ s/^"//;
		$fileName =~ s/"$//;
		$outLine = "layout_path2 \"$fileName\";\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+path/is) {
		my ($fileName) = ($inLine =~ /^\s*layout\s+path\s+(\S+)/is);
		$fileName =~ s/^"//;
		$fileName =~ s/"$//;
		$outLine = "layout_path \"$fileName\";\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+system2/is) {
		my ($system) = ($inLine =~ /^\s*layout\s+system2\s+(\S+)/is);
		$system =~ s/^"//;
		$system =~ s/"$//;
		$outLine = "layout_format2 $system;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+system/is) {
		my ($system) = ($inLine =~ /^\s*layout\s+system\s+(\S+)/is);
		$system =~ s/^"//;
		$system =~ s/"$//;
		$outLine = "layout_format $system;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+primary2/is) {
		my ($primary) = ($inLine =~ /^\s*layout\s+primary2\s+(\S+)/is);
		$primary =~ s/^"//;
		$primary =~ s/"$//;
		$outLine = "layout_primary2 \"$primary\";\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+primary/is) {
		my ($primary) = ($inLine =~ /^\s*layout\s+primary\s+(\S+)/is);
		$primary =~ s/^"//;
		$primary =~ s/"$//;
		$outLine = "layout_primary \"$primary\";\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*svrf\s+message.*/is) {
		my ($remain) = ($inLine =~ /^\s*svrf\s+message(.*)$/is);
		$outLine = "vlf_message $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*flatten\s+cell.*/is) {
		my ($remain) = ($inLine =~ /^\s*flatten\s+cell(.*)$/is);
		$outLine = "flatten_cell $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+cell\s+list\b.*/is) {
		my ($remain) = ($inLine =~ /^\s*layout\s+cell\s+list(.*)$/is);
		$outLine = "layout_cell_list $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+check\s+text/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+check\s+text\s+(\S+)/is);
		$remain =~ s/^"//;
		$remain =~ s/"$//;
		my $option = "";	
		if ($remain =~ /all/is) {
			$option .= "-all";
		}
		$outLine = "check_text -drc $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+keep\s+empty\s+\S+/is) {
		my ($word) = ($inLine=~ /^\s*drc\s+keep\s+empty\s+(\S+)/is);
		$outLine = "keep_empty -drc $word;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+select\s+check\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+select\s+check(.*)$/is);
		$outLine = "select_check -drc $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+unselect\s+check\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+unselect\s+check(.*)$/is);
		$outLine = "unselect_check -drc $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+cell\s+name\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+cell\s+name\s+(.*)$/is);
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^yes/is) {
				$remain =~ s/^yes//is;
				$option .= " yes";
			} elsif ($remain =~ /^no/is) {
				$remain =~ s/^no//is;
				$option .= " no";
			} elsif ($remain =~ /^cell\s+space\b/is) {
				$remain =~ s/^cell\s+space//is;
				$option .= " -cell_space";
			} elsif ($remain =~ /^xform/is) {
				$remain =~ s/^xform//is;
				$option .= " -xform";
			} elsif ($remain =~ /^all/is) {
				$remain =~ s/^all//is;
				$option .= " -all";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = "cell_name -drc $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+results\s+database\s+/is) {
		my ($spec) = ($inLine =~ /^\s*drc\s+results\s+database\s+(.*)$/is);
		my ($outFile, $remain) = ($spec =~ /^(\S+)(.*)/);
		$outFile =~ s/^"//;
		$outFile =~ s/"$//;
		my $format = "-binary";
		if ($remain =~ /\boasis\b/is) {
			$format = "-oasis";
		} elsif ($remain =~ /\bgdsii\b/is or
				 $remain =~ /\bgds2\b/is or
				 $remain =~ /\bgds\b/) {
			$format = "-gdsii";
		} elsif ($remain =~ /\bascii\b/is) {
			$format = "-ascii";
		}	
		my $pseudo = "";
		if ($remain =~ /\bpseudo\b/is) {
			$pseudo = "-pseudo";
		}
		my $option = "";
		if ($remain =~ /\bcblock\b/is) {
			$option .= " -cblock";
		}
		if ($remain =~ /\bprefix\s+\S+/is) {
			my ($key, $word) = ($remain =~ /\b(prefix)\s+(\S+)/);
			$option .= " -prefix $word";
		}
		if ($remain =~ /\bappend\s+\S+/is) {
			my ($key, $word) = ($remain =~ /\b(append)\s+(\S+)/);
			$option .= " -append $word";
		}
		$outLine = "results_db -drc \"$outFile\" $format $option $pseudo;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+summary\s+report\s+/is) {
		my ($spec) = ($inLine =~ /^\s*drc\s+summary\s+report\s+(.*)$/is);
		my ($outFile, $remain) = ($spec =~ /^(\S+)(.*)/);
		$outFile =~ s/^"//;
		$outFile =~ s/"$//;
		my $hier = "";
		if ($remain =~ /hier/is) {
			$hier = "-hier";
		}
		$outLine = "report_summary -drc \"$outFile\" $hier;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+exclude\s+false\s+notch\s+/is) {
		#no effect
		return ("", 0);
	} elsif ($inLine =~ /^\s*erc\s+summary\s+report\s+/is) {
		my ($spec) = ($inLine =~ /^\s*erc\s+summary\s+report\s+(.*)$/is);
		my ($outFile, $remain) = ($spec =~ /^(\S+)(.*)/);
		$outFile =~ s/^"//;
		$outFile =~ s/"$//;
		my $hier = "";
		if ($remain =~ /hier/is) {
			$hier = "-hier";
		}
		$outLine = "report_summary -erc \"$outFile\" $hier;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layer\s+MAP\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*layer\s+map\s+(.*)$/is);
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^[<>=!]/) {
				($word, $remain) = ($remain=~/^(\S+)(.*)$/);
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					print "!!CHK_ERR<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} elsif ($remain =~ /^datatype\b/is) {
				($word, $remain) = ($remain=~/^(\S+)(.*)$/);
				$option .= " -datatype";
			} elsif ($remain =~ /^texttype\b/is) {
				($word, $remain) = ($remain=~/^(\S+)(.*)$/);
				$option .= " -texttype";
			} else {
				($word, $remain) = ($remain=~/^(\S+)(.*)$/);
				$option .= " $word";
			}
		}
		$outLine = "layer_map $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layer\s+resolution\s+/is) {
		my ($layer, $res) = ($inLine =~ /^\s*layer\s+resolution\s+(\S+)\s+(\S+)/is);
		$outLine = "layer_grid $layer $res;\n"; # not support in vela parser
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layer\s+\S+\s+\S+/is) {
		my ($layName,$layId) = ($inLine =~ /^\s*layer\s+(\S+)\s+(.*)$/is);
		$outLine = "layer_def $layName $layId;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*source\s+path/is) {
		my ($path) = ($inLine =~ /^\s*source\s+path\s+(.*)$/is);
		$outLine = "schematic_path $path;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*source\s+primary/is) {
		my ($path) = ($inLine =~ /^\s*source\s+primary\s+(.*)$/is);
		$outLine = "schematic_primary $path;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*source\s+system/is) {
		my ($path) = ($inLine =~ /^\s*source\s+system\s+(.*)$/is);
		$outLine = "schematic_system $path;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*lvs\s+report\s+option/is) {
		my ($file) = ($inLine =~ /^\s*lvs\s+report\s+option\s+(.*)$/is);
		$outLine = "lvs_report_option $file;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*lvs\s+report\s+maximum/is) {
		my ($file) = ($inLine =~ /^\s*lvs\s+report\s+maximum\s+(.*)$/is);
		$outLine = "lvs_report_maximum $file;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*lvs\s+report\s+/is) {
		my ($file) = ($inLine =~ /^\s*lvs\s+report\s+(.*)$/is);
		$outLine = "lvs_report_file $file;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*lvs\s+power\s+name\s+/is) {
		my ($file) = ($inLine =~ /^\s*lvs\s+power\s+name\s+(.*)$/is);
		$outLine = "lvs_power_name $file;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*lvs\s+ground\s+name\s+/is) {
		my ($file) = ($inLine =~ /^\s*lvs\s+ground\s+name\s+(.*)$/is);
		$outLine = "lvs_ground_name $file;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*lvs\s+execute\s+erc\s+/is) {
		my ($file) = ($inLine =~ /^\s*lvs\s+execute\s+erc\s+(.*)$/is);
		$outLine = "lvs_exec_erc $file;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*lvs\s+isolate\s+short\s+/is) {
		my ($file) = ($inLine =~ /^\s*lvs\s+isolate\s+short\s+(.*)$/is);
		$outLine = "lvs_found_shorts $file;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*hcell\s+\S+\s+\S+/is) {
		my ($layoutName, $sourceName) = ($inLine =~ /^\s*hcell\s+(\S+)\s+(\S+)/is);
		$outLine = "hcell $layoutName $sourceName;\n";
		return ($outLine, 0);
	} elsif ($refFlag->{inRule} == 0 && $inLine =~ /^\s*(\S+)\s*{\s*$/is) {
		my ($rule) = ($inLine =~ /^\s*(\S+)\s*{/);
		if ($rule =~ /tvf::VERBATIM/i) {
			$outLine = "tvf::VERBATIM {\n";
			#$outLine = "VRF::RULE_BLOCK {\n";
			$refFlag->{inTvf} = 0;
			return ($outLine, 0);
		} else {
			$outLine = "rule $rule {\n";
			
			$refFlag->{inRule} = 1;
			$refFlag->{inRuleCap} = 1;
			return ($outLine, 0);
		}
	} elsif ($isTvf == 1 and 
		$refFlag->{inTvf} == 0 and
		$refFlag->{inRule} == 0 and
		$inLine =~ /^\s*}\s*$/) {
		$refFlag->{inTvf} = 1;
		$outLine = "} // end of VRF::RULE_BLOCK\n\n";
		return ($outLine, 0);
	} elsif ($refFlag->{inRule} == 1 && $inLine =~ /^\s*}\s*$/) {
		$refFlag->{inRule} = 0;
		$outLine = "}\n";
		return ($outLine, 0);	
	} elsif ($refFlag->{inRule} == 1 && $inLine =~ /^\s*@.*$/) {
		$outLine = $inLine;
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+map\s+text\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+map\s+text\s+(.*)/is);
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^depth\s+\S+/is) {
				($word, $remain) = ($remain=~/^depth\s+(\S+)(.*)$/is);	
				$option .= " -depth $word";
			} elsif ($remain =~ /^layer\s+\S+/is) {
				($word, $remain) = ($remain=~/^layer\s+(\S+)(.*)$/is);	
				$option .= " -layer $word";
			} elsif ($remain =~ /^ignore\s+\S+/is) {
				($word, $remain) = ($remain=~/^ignore\s+(\S+)(.*)$/is);	
				$option .= " -ignore $word";
			} elsif ($remain =~ /^yes\b/is) {
				$remain =~ s/^yes//is;
				$option .= " yes";
			} elsif ($remain =~ /^no\b/is) {
				$remain =~ s/^no//is;
				$option .= " no";
			} else {
				($word, $remain) = ($remain=~/^(\S+)(.*)$/is);	
				$option .= " $word";
			}
		}
		$outLine = "map_text $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+magnify\s+\S+/is) {
		my ($word, $remain) = ($inLine =~ /^\s*(layout\s+magnify)\s+(.*)$/is);
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/\s*//;
			if ($remain =~ /^\s*place\s+/is) {
				$remain =~ s/^\s*place\s+//is;
				$option .= " -place";
			} elsif ($remain =~ /^\s*auto\s+/is) {
				$remain =~ s/^\s*auto\s+//is;
				$option .= " auto";
			} else {
				($word, $remain) = ($remain =~ /^\s*(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = "layout_zoom $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+process\s+box\s+record\s+\S+/is) {
		my ($word) = ($inLine =~ /^\s*layout\s+process\s+box\s+record\s+(\S+)/is);
		$outLine = "layout_include_box_record $word;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+check\s+map\s+\S+/is) {
		my ($rule, $remain) = ($inLine =~ /^\s*drc\s+check\s+map\s+(\S+)\s+(.*)$/is);
		my $format = "";
		my $word = "";
		my $option = "";
		my ($layer, $type) = ("", "");
		my $stage = 0; # 1 for layer/datatype, 2 for file | pipe
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^(gdsii|gds|gds2)/is) {
				$remain =~ s/^(gdsii|gds|gds2)//is;
				$format = "gdsii";
			} elsif ($remain =~ /^oasis/is) {
				$remain =~ s/oasis//is;
				$format = "oasis";
			} elsif ($remain =~ /^ascii/is) {
				$remain =~ s/ascii//is;
				$format = "ascii";
				$stage = 1;
			} elsif ($remain =~ /^binary/is) {
				$remain =~ s/binary//is;
				$format = "binary";
				$stage = 1;
			} elsif ($remain =~ /^properties/is) {
				$remain =~ s/properties//is;
				$option .= " properties";
				$stage = 1;
			} elsif ($remain =~ /^prefix/is) {
				$remain =~ s/prefix//is;
				$option .= " -prefix";
			} elsif ($remain =~ /^append/is) {
				$remain =~ s/append//is;
				$option .= " -append";
			} elsif ($remain =~ /^maximum\s+results\s+\S+/is) {
				($word, $remain) = ($remain =~ /^maximum\s+results\s+(\S+)(.*)$/is);
				if ($word =~ /all/is) {
					$word = "all";
				}
				$option .= " -maximum $word";
			} elsif ($remain =~ /^maximum\s+vertices\s+\S+/is) {
				($word, $remain) = ($remain =~ /^maximum\s+vertices\s+(\S+)(.*)$/is);
				if ($word =~ /all/is) {
					$word = "all";
				}
				$option .= " -max_vertices $word";
			} elsif ($remain =~ /^texttag\s+\S+/is) {
				($word, $remain) = ($remain =~ /^texttag\s+(\S+)(.*)$/is);
				$option .= " -texttag $word";
			} elsif ($remain =~ /^user\s+merged/is) {
				$remain =~ s/^user\s+merged//is;
				$option .= " -user_merged";
			} elsif ($remain =~ /^pseudo\b/is) {
				$remain =~ s/^pseudo//is;
				$option .= " -pseudo";
			} elsif ($remain =~ /^user\b/is) {
				$remain =~ s/^user//is;
				$option .= " -user";
			} elsif ($remain =~ /^prec\s+\S+/is) {
				($word, $remain) = ($remain =~ /^prec\s+(\S+)(.*)$/is);
				$option .= " -prec $word";
			} elsif ($remain =~ /^mag\s+\S+/is) {
				($word, $remain) = ($remain =~ /^mag\s+(\S+)(.*)$/is);
				$option .= " -mag $word";
			} elsif ($remain =~ /^index\b/is) {
				$remain =~ s/^index//is;
				$option .= " -index";
			} elsif ($remain =~ /^noview\b/is) {
				$remain =~ s/^noview//is;
				$option .= " -noview";
			} elsif ($remain =~ /^aref\b/is) {
				$remain =~ s/^aref//is;
				$option .= " -aref";
			} elsif ($remain =~ /^maximum\s+pitch\b/is) {
				$remain =~ s/^maximum\s+pitch//is;
				$option .= " -maximum_pitch";
			} elsif ($remain =~ /^substitute\b/is) {
				$remain =~ s/^substitute//is;
				$option .= " -substitute";
			} elsif ($remain =~ /^autoref\b/is) {
				$remain =~ s/^autoref//is;
				$option .= " -autoref";
			} elsif ($remain =~ /^\d+\b/is) {
				($word, $remain) = ($remain=~ /^(\d+)(.*)$/is);
				if ($stage == 0) {
					if ($format eq "") {
						$format = "gdsii";
					}
					if ($layer eq "") {
						$layer = $word;
					} elsif ($type eq "") {
						$type = $word;
						$stage = 1;
					} else {
						$option .= " $word";
					}
				}
			} elsif ($remain =~ /^"pipe\s+.*"/) {
				($word, $remain) = ($remain =~ /^("pipe.*")(.*)/is);
				$option .= " $word";
			} elsif ($remain =~ /^".*?"/) {
				($word, $remain) = ($remain =~ /^(".*?")(.*)/is);
				$option .= " $word";
			} else {
				($word, $remain) = ($remain=~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		if ($format eq "gdsii" or $format eq "oasis") {
			$format = " -$format $layer $type";
		} else {
			$format = " -$format";
		}
		$outLine = "output -drc $rule $format $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*outputLayer\s+\S+/is) {
		my ($rule, $remain) = ($inLine =~ /^\s*outputLayer\s+(\S+)\s+(.*)$/is);
		my $format = "";
		my $word = "";
		my $option = "";
		my ($layer, $type) = ("", "");
		my $stage = 0; # 1 for layer/datatype, 2 for file | pipe
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^(gdsii|gds|gds2)/is) {
				$remain =~ s/^(gdsii|gds|gds2)//is;
				$format = "gdsii";
			} elsif ($remain =~ /^oasis/is) {
				$remain =~ s/oasis//is;
				$format = "oasis";
			} elsif ($remain =~ /^ascii/is) {
				$remain =~ s/ascii//is;
				$format = "ascii";
				$stage = 1;
			} elsif ($remain =~ /^binary/is) {
				$remain =~ s/binary//is;
				$format = "binary";
				$stage = 1;
			} elsif ($remain =~ /^properties/is) {
				$remain =~ s/properties//is;
				$option .= " properties";
				$stage = 1;
			} elsif ($remain =~ /^prefix/is) {
				$remain =~ s/prefix//is;
				$option .= " -prefix";
			} elsif ($remain =~ /^append/is) {
				$remain =~ s/append//is;
				$option .= " -append";
			} elsif ($remain =~ /^maximum\s+results\s+\S+/is) {
				($word, $remain) = ($remain =~ /^maximum\s+results\s+(\S+)(.*)$/is);
				if ($word =~ /all/is) {
					$word = "all";
				}
				$option .= " -maximum $word";
			} elsif ($remain =~ /^maximum\s+vertices\s+\S+/is) {
				($word, $remain) = ($remain =~ /^maximum\s+vertices\s+(\S+)(.*)$/is);
				if ($word =~ /all/is) {
					$word = "all";
				}
				$option .= " -max_vertices $word";
			} elsif ($remain =~ /^texttag\s+\S+/is) {
				($word, $remain) = ($remain =~ /^texttag\s+(\S+)(.*)$/is);
				$option .= " -texttag $word";
			} elsif ($remain =~ /^user\s+merged/is) {
				$remain =~ s/^user\s+merged//is;
				$option .= " -user_merged";
			} elsif ($remain =~ /^pseudo\b/is) {
				$remain =~ s/^pseudo//is;
				$option .= " -pseudo";
			} elsif ($remain =~ /^user\b/is) {
				$remain =~ s/^user//is;
				$option .= " -user";
			} elsif ($remain =~ /^prec\s+\S+/is) {
				($word, $remain) = ($remain =~ /^prec\s+(\S+)(.*)$/is);
				$option .= " -prec $word";
			} elsif ($remain =~ /^mag\s+\S+/is) {
				($word, $remain) = ($remain =~ /^mag\s+(\S+)(.*)$/is);
				$option .= " -mag $word";
			} elsif ($remain =~ /^index\b/is) {
				$remain =~ s/^index//is;
				$option .= " -index";
			} elsif ($remain =~ /^noview\b/is) {
				$remain =~ s/^noview//is;
				$option .= " -noview";
			} elsif ($remain =~ /^aref\b/is) {
				$remain =~ s/^aref//is;
				$option .= " -aref";
			} elsif ($remain =~ /^maximum\s+pitch\b/is) {
				$remain =~ s/^maximum\s+pitch//is;
				$option .= " -maximum_pitch";
			} elsif ($remain =~ /^substitute\b/is) {
				$remain =~ s/^substitute//is;
				$option .= " -substitute";
			} elsif ($remain =~ /^autoref\b/is) {
				$remain =~ s/^autoref//is;
				$option .= " -autoref";
			} elsif ($remain =~ /^\d+\b/is) {
				($word, $remain) = ($remain=~ /^(\d+)(.*)$/is);
				if ($stage == 0) {
					if ($format eq "") {
						$format = "gdsii";
					}
					if ($layer eq "") {
						$layer = $word;
					} elsif ($type eq "") {
						$type = $word;
						$stage = 1;
					} else {
						$option .= " $word";
					}
				}
			} elsif ($remain =~ /^"pipe\s+.*"/) {
				($word, $remain) = ($remain =~ /^("pipe.*")(.*)/is);
				$option .= " $word";
			} elsif ($remain =~ /^".*?"/) {
				($word, $remain) = ($remain =~ /^(".*?")(.*)/is);
				$option .= " $word";
			} else {
				($word, $remain) = ($remain=~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		if ($format eq "gdsii" or $format eq "oasis") {
			$format = " -$format $layer $type";
		} else {
			$format = " -$format";
		}
		$outLine = "outputLayer $rule $format $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+text\s+/is) {
		my ($content) = ($inLine =~ /^\s*layout\s+text\s+(.*)$/is);
		$outLine = "layout_text $content;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\s*title\s+/is) {
		my ($title) = ($inLine =~ /\s*title\s+(.*)$/is);
		$outLine = "title $title;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+precision\s+/is) {
		my ($precision) = ($inLine =~ /^\s*layout\s+precision\s+(\S+)/is);
		$outLine = "input_db_scale $precision;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*precision\s+/is) {
		my ($precision) = ($inLine =~ /^\s*precision\s+(\S+)/is);
		$outLine = "input_db_scale $precision;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*resolution\s+/is) {
		my ($res) = ($inLine =~ /^\s*resolution\s+(\S+)/is);
		$outLine = "grid $res;\n"; # not support in vela parser
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+depth\s+/is) {
		my ($depth) = ($inLine =~ /^\s*layout\s+depth\s+(\S+)/is);
		if ($depth =~ /all/i) {
			$depth = " -all";
		} elsif ($depth =~ /primary/i) {
			$depth = " -primary";
		}
		$outLine = "layout_depth $depth;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+input\s+exception\s+rdb\s+/is) {
		my ($file, $remain) = ($inLine=~/^\s*layout\s+input\s+exception\s+rdb\s+(\S+)(.*)$/is);
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^by\s+cell\b/is) {
				$remain =~ s/^by\s+cell//;
				$option .= " -by_cell";
			} elsif ($remain =~ /^by\s+layer\b/is) {
				$remain =~ s/^by\s+layer//;
				$option .= " -by_layer";
			} elsif ($remain =~ /^by\s+exception\b/is) {
				$remain =~ s/^by\s+exception//;
				$option .= " -by_exception";
			} elsif ($remain =~ /^top\s+space\b/is) {
				$remain =~ s/^top\s+space//;
				$option .= " -top_space";
			} elsif ($remain =~ /^cell\s+space\b/is) {
				$remain =~ s/^cell\s+space//;
				$option .= " -cell_space";
			} elsif ($remain =~ /^exit\s+nonempty\b/is) {
				$remain =~ s/^exit\s+nonempty//;
				$option .= " -exit_nonempty";
			} elsif ($remain =~ /^mag\s+\S+/is) {
				($word, $remain) = ($remain=~/^mag\s+(\S+)(.*)$/is);
				$option .= " -mag $word";
			} else {
				($word, $remain) = ($remain=~/^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = "exception_rdb $file $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+input\s+exception\s+severity\s+/is) {
		my ($remain) = ($inLine=~/^\s*layout\s+input\s+exception\s+severity\s+(.*)$/is);
		my $option = "";
		my ($key, $word) = ("", "");
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^ARRAY_PITCH_ZERO\s+\S+/is) {
				($word, $remain) = ($remain =~ /^ARRAY_PITCH_ZERO\s+(\S+)(.*)$/is);
				$option .= " array_pitch_zero $word";
			} elsif ($remain =~ /^aref_angled\s+\S+/is) {
				($word, $remain) = ($remain =~ /^aref_angled\s+(\S+)(.*)$/is);
				$option .= " aref_angled $word";
			} elsif ($remain =~ /^aref_placement\s+\S+/is) {
				($word, $remain) = ($remain =~ /^aref_placement\s+(\S+)(.*)$/is);
				$option .= " aref_placement $word";
				
			} else {
				if ($remain =~ /^(\S+)\s+(\S+)(.*)$/is) {
					($key, $word, $remain) = ($remain =~ /^(\S+)\s+(\S+)(.*)$/is);
					$key = lc $key;
					$option .= " $key $word";
				} else {
					$remain =~ s/\s*$//;
					$option .= "<<<$remain>>";
					$remain = "";
				}
				
			}
		} 
		$outLine = "exception_severity $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+allow\s+duplicate\s+cell\s+/is) {
		my ($remain) = ($inLine =~ /^\s*layout\s+allow\s+duplicate\s+cell\s+(.*)$/is);
		$outLine = "input_allow_duplicate_cell $remain;\n";
		return ($outLine, 0);	
	} elsif ($inLine =~ /^\s*DRC\s+boolean\s+nosnap45/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+boolean\s+nosnap45\s+(\S+)/is);
		$outLine = "// nosnap45 $remain // no effect;\n";
		return ($outLine, 0);	
	} elsif ($inLine =~ /^\s*drc\s+print\s+perimeter\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+print\s+perimeter\s+(.*)$/is);
		$outLine = "print_perim $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+print\s+area\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+print\s+area\s+(.*)$/is);
		$outLine = "print_area $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+print\s+extent\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+print\s+extent\s+(.*)$/is);
		$outLine = "print_extent $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+maximum\s+results\s+/is) {
		my ($max) = ($inLine =~ /^\s*drc\s+maximum\s+results\s+(\S+)/i);
		$outLine = "max_results -drc $max;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*erc\s+maximum\s+results\s+/is) {
		my ($max) = ($inLine =~ /^\s*erc\s+maximum\s+results\s+(\S+)/i);
		$outLine = "max_results -erc $max;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*text\s+print\s+maximum\s+/is) {
		my ($max) = ($inLine =~ /^\s*text\s+print\s+maximum\s+(\S+)/i);
		if ($max =~ /all/is) {
			$max = "-all";
		}
		$outLine = "text_print_max $max;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+maximum\s+vertex\s+/is) {
		my ($max) = ($inLine =~ /^\s*drc\s+maximum\s+vertex\s+(\S+)/i);
		$outLine = "max_vertex -drc $max;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*erc\s+maximum\s+vertex\s+/is) {
		my ($max) = ($inLine =~ /^\s*erc\s+maximum\s+vertex\s+(\S+)/i);
		$outLine = "max_vertex -erc $max;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+maximum\s+cell\s+name\s+length\s+/is) {
		my ($length) = ($inLine =~ /^\s*drc\s+maximum\s+cell\s+name\s+length\s+(\S+)/i);
		$outLine = "max_cname_len $length;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+tolerance\s+factor\s+/is) {
		my ($tol) = ($inLine =~ /^\s*drc\s+tolerance\s+factor\s+(\S+)/is);
		$outLine = "tolerance $tol;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*drc\s+incremental\s+connect\s+/is) {
		my ($word) = ($inLine =~ /^\s*drc\s+incremental\s+connect\s+(\S+)/is);
		$word = lc $word;
		$outLine = "incr_conn $word;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\brotate\s+\S+.*by/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "rotate");
		my $value = "";
		my $haveCons = 0;
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^BY\b/) {
				($value, $remain) = ($remain =~ /^BY\s+(\S+)(.*)$/is);
			} else {
				$value .= $remain;
				$remain = "";
			}
		}
		$outLine = &genOutLine("rotate"," $layer -angle $value $option", $outLay, 1);
		return ($outLine, 0);
	
	} elsif ($inLine =~ /^\s*drc\s+magnify\s+/is) {
		my ($remain) = ($inLine =~ /^\s*drc\s+magnify(.*)$/is);
		my $value = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^results\s+\S+/is) {
				($value, $remain) = ($remain =~ /^results\s+(\S+)(.*)$/is);
				$option .= " -results $value";
			} elsif ($remain =~ /^density\s+\S+/is) {
				($value, $remain) = ($remain =~ /^density\s+(\S+)(.*)$/is);
				$option .= " -density $value";
			} elsif ($remain =~ /^nar\s+\S+/is) {
				($value, $remain) = ($remain =~ /^nar\s+(\S+)(.*)$/is);
				$option .= " -nar $value";
			} else {
				($value, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $value";
			}
		}
		$outLine = "magnify_out $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*dmacro\s+\S+.*\{/is) {
		my ($macroName, $macroArg) = ($inLine =~ /^\s*dmacro\s+(\S+)(.*){/is);
		$outLine = "dmacro $macroName $macroArg {\n";
		$refFlag->{inRule} = 1;
		$refFlag->{inRuleCap} = 1;
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*cmacro\s+\S+.*/is) {
		my ($macroName, $macroArg) = ($inLine =~ /^\s*cmacro\s+(\S+)(.*)$/is);
		$outLine = "cmacro $macroName $macroArg;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+space\s+\S+/is) {
		my ($outLay, $layers, $remain) = 
			&parseCmdWithLayer($inLine, "dfm\\s+space");
		my $word = "";
		my $haveCons = 0;
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($haveCons == 0) {
				if ($remain =~ /^[><=!]/) {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
					my ($isOp, $resOp) = &transRelOp($word);
					$option .= " $resOp";
					$haveCons = 1;
				} else {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
					$layers .= " $word";
				}
			} else { #haveCons = 1;
				if ($remain =~ /^[><=!]/) {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
					my ($isOp, $resOp) = &transRelOp($word);
					$option .= " $resOp";
					$haveCons = 1;
				} elsif ($remain =~ /^BY\s+(ext|INT|enc)/is) {
					my $key = "";
					($word,$remain) = ($remain =~ /^BY\s+(\S+)(.*)$/is);
					my $outKey = lc $word;	
					my $outWord = "-by_".$outKey;
					$option .= " $outWord";
				} elsif ($remain =~ /^all\b/is) {
					$remain =~ s/^all//is;
					$option .= " -by_all";
				} elsif ($remain =~ /^HORIZONTAL/is) {
					$remain =~ s/^HORIZONTAL//is;	
					$option .= " -horizontal";
				} elsif ($remain =~ /^VERTICAL/is) {
					$remain =~ s/^VERTICAL//is;	
					$option .= " -vertical";
				} elsif ($remain =~ /^COUNT\s+\S+/is) {
					($word,$remain) = ($remain =~ /^COUNT\s+(\S+)(.*)$/is);
					my ($isOp, $resOp) = &transRelOp($word);
					my $outWord = "-count $resOp";
					$option .= " $outWord";
				} elsif ($remain =~ /^BY\s+LAYER\s+\S+/is) {
					($word, $remain) = ($remain =~ /^BY\s+LAYER\s+(\S+)(.*)$/is);
					my $outWord = "-by_layer $word";
					$option .= " $outWord";
				} elsif ($remain =~ /^CONNECTED/is) {
					$remain =~ s/^CONNECTED//;
					$option .= " -connected";
				} elsif ($remain =~ /^NOT\s+CONNECTED/is) {
					$remain =~ s/^NOT\s+CONNECTED//;
					$option .= " -not_connected";
				} elsif ($remain =~ /^MEASURE\s+ALL/is) {
					$remain =~ s/^MEASURE\s+ALL//is;
					$option .= " -measure all";
				} elsif ($remain =~ /^GRID\s+\S+/is) {
					($word, $remain) = ($remain =~ /^GRID\s+(\S+)(.*)$/is);
					$option .= " -grid $word";
				} else {
					($word, $remain) = ($remain =~ /(\S+)(.*)$/is);
					$option .= " $word";
				}
			}
		}
		$outLine = &genOutLine("dfm_space"," $layers $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+copy\s+/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "dfm\\s+copy");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/is) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^REGION\s+UNMERGED/is) {
				$remain =~ s/^region\s+unmerged//is;
				$option .= " -output region -unmerged";
			} elsif ($remain =~ /^REGION\s+/is) {
				$remain =~ s/^region//is;
				$option .= " -output region";
			} elsif ($remain =~ /^edge\s+/is) {
				$remain =~ s/^edge//is;
				$option .= " -output edge";
			} elsif ($remain =~ /^centerline\s+/is) {
				$remain =~ s/^centerline//is;
				$option .= " -output centerline";
			} elsif ($remain =~ /^tie\s+center\b/is) {
				$remain =~ s/^tie\s+center//is;
				$option .= " -output center_tie";
			} elsif ($remain =~ /^tie\s+/is) {
				$remain =~ s/^tie//is;
				$option .= " -output tie";
			} elsif ($remain =~ /^cluster\s+layerid\b/is) {
				$remain =~ s/^cluster\s+layerid//is;
				$option .= " -output cluster -layerid";
			} elsif ($remain =~ /^cluster\b/is) {
				$remain =~ s/^cluster//is;
				$option .= " -output cluster";
			} elsif ($remain =~ /^unmerged\b/is) {
				$remain =~ s/^unmerged//is;
				$option .= " -allow_unmerged_input";
			} elsif ($remain =~ /^cell\s+list\s+\S+/is) {
				($word) = ($remain =~ /^cell\s+list\s+(.*)$/is);
				$option .= " -cell_list $word";
				$remain = "";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("dfm_copy"," $layer $option", $outLay, 2);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+dv\s+\S+/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "dfm\\s+dv");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/is) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^DVPARAMS\s+\S+/) {
				my $parLay;
				($parLay) = ($remain =~ /DVPARAMS\s+(\S+)/);
				$remain =~ s/^DVPARAMS\s+\S+//;
				$option .= " -dvparams $parLay";
			} elsif ($remain =~ /^connected/is) {
				$remain =~ s/^connected//is;
				$option .= " -connected";
			} elsif ($remain =~ /^not\s+connected/is) {
				$remain =~ s/^not\s+connected//is;
				$option .= " -not_connected";
			} elsif ($remain =~ /^singular/is) {
				$remain =~ s/^singular//is;
				$option .= " -single_point";
			} elsif ($remain =~ /^annotate\b/is) {
				$remain =~ s/^annotate//is;
				$option .= " -annotate";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}	
		}
		$outLine = &genOutLine("dfm_dv", "$layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+expand\s+edge\s+\S+/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "dfm\\s+expand\\s+edge");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/is) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^outside\s+by\b/is) {
				$remain =~ s/^outside\s+by//is;
				$option .= " -outside_by";
			} elsif ($remain =~ /^inside\s+by\b/is) {
				$remain =~ s/^inside\s+by//is;
				$option .= " -inside_by";
			} elsif ($remain =~ /^extend\s+by\b/is) {
				$remain =~ s/^extend\s+by//is;
				$option .= " -extend_by";
			} elsif ($remain =~ /^offset\b/is) {
				$remain =~ s/^offset//is;
				$option .= " -offset";
			} elsif ($remain =~ /^unmerged\b/is) {
				$remain =~ s/^unmerged//is;
				$option .= " -unmerged";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}		
		}
		$outLine = &genOutLine("dfm_edge_expand"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+grow\s+\S+/is) {
		my ($outLay, $layer, $remain) = &parseCmdWithLayer($inLine, "dfm\\s+grow");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^targetarea\b/is) {
				$remain =~ s/^targetarea//is;
				$option .= " -target_area";
			} elsif ($remain =~ /^minwidth\b/is) {
				$remain =~ s/^minwidth//is;
				$option .= " -min_width";
			} elsif ($remain =~ /^space\b/is) {
				$remain =~ s/^space//is;
				$option .= " -space";
			} elsif ($remain =~ /^step\b/is) {
				$remain =~ s/^step//is;
				$option .= " -step";
			} elsif ($remain =~ /^iterations\b/is) {
				$remain =~ s/^iterations//is;
				$option .= " -iterations";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}	
		$outLine = &genOutLine("dfm_grow"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+property\s+merge\s+(\S+)/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "dfm\\s+property\\s+merge");
		my $word = "";
		my $option = "";
		my $opt = 0;
		my $propExpr = "";
		my $cons = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^intersecting/is) {
				$remain =~ s/^intersecting//is;
				$option .= " -intersecting";
				$opt = 1;
			} elsif ($remain =~ /^overlap/is) {
				$remain =~ s/^overlap//is;
				$option .= " -overlap";
				$opt = 1;
			} elsif ($remain =~ /^abut\s+also/is) {
				$remain =~ s/^abut\s+also//is;
				$option .= " -abut_also";
				$opt = 1;
			} elsif ($remain =~ /^singular/is) {
				$remain =~ s/^singular//is;
				$option .= " -single_point";
				$opt = 1;
			} elsif ($remain =~ /^multi/is) {
				$remain =~ s/^multi//is;
				$option .= " -multi";
				$opt = 1;
			} elsif ($remain =~ /^nomulti/is) {
				$remain =~ s/^nomulti//is;
				$option .= " -nomulti";
				$opt = 1;
			} elsif ($remain =~ /^nodal/is) {
				$remain =~ s/^nodal//is;
				$option .= " -nodal";
				$opt = 1;
			} elsif ($remain =~ /^corner/is) {
				$remain =~ s/^corner//is;
				$option .= " -corner";
				$opt = 1;
			} elsif ($remain =~ /^connected/is) {
				$remain =~ s/^connected//is;
				$option .= " -connected";
				$opt = 1;
			} elsif ($remain =~ /^not\s+connected/is) {
				$remain =~ s/^not\s+connected//is;
				$option .= " -not_connected";
				$opt = 1;
			} elsif ($remain =~ /^inside\s+of\s+\S+\s+\S+\s+\S+\s+\S+/is) {
				my ($x1, $y1, $x2, $y2);
				($x1, $y1, $x2, $y2, $remain) = 
					($remain =~ /^inside\s+of\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)(.*)$/is);
				$option .= " -inside_of $x1 $y1 $x2 $y2";
				$opt = 1;
			} elsif ($remain =~ /^split\s+all/is) {
				$remain =~ s/^split\s+all//is;
				$option .= " -split_all";
				$opt = 1;
			} elsif ($remain =~ /^split/is) {
				$remain =~ s/^split//is;
				$option .= " -split";
				$opt = 1;
			} elsif ($remain =~ /^accumulate\s+only/is) {
				$remain =~ s/^accumulate\s+only//is;
				$option .= " -accumulate_only";
				$opt = 1;
			} elsif ($remain =~ /^accumulate/is) {
				$remain =~ s/^accumulate//is;
				$option .= " -accumulate";
				$opt = 1;
			} elsif ($remain =~ /^by\s+net\s+only/is) {
				$remain =~ s/^by\s+net\s+only//is;
				$option .= " -by_net_only";
				$opt = 1;
			} elsif ($remain =~ /^by\s+net/is) {
				$remain =~ s/^by\s+net//is;
				$option .= " -by_net_only";
				$opt = 1;
			} elsif ($remain =~ /^nohier/is) {
				$remain =~ s/^nohier//is;
				$option .= " -no_hier";
				$opt = 1;
			} elsif ($remain =~ /^nopseudo/is) {
				$remain =~ s/^nopseudo//is;
				$option .= " -no_pseudo";
				$opt = 1;
			} elsif ($remain =~ /^by\s+cell\s+only/is) {
				$remain =~ s/^by\s+cell\s+only//is;
				$option .= " -by_cell_only";
				$opt = 1;
			} elsif ($remain =~ /^by\s+cell/is) {
				$remain =~ s/^by\s+cell//is;
				$option .= " -by_cell";
				$opt = 1;
			} else {
				if ($remain =~ /^\[[^\]]+\]/is) {
					($word, $remain) = ($remain =~ /^(\[[^\]]+\])(.*)$/is);
					$propExpr .= " $word";
					$opt = 1;
				} elsif ($remain =~ /^[<>=!]/) {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					my ($isOp, $resOp) = &transRelOp($word);
					if ($isOp > 0) {
						$propExpr .= " $resOp";
						if ($propExpr !~ /(gt|ge|lt|le|ne|eq)$/) {
							$propExpr .= "\n";
						}
						$cons = " $resOp";
					} else {
						print "!!CHK_ERR<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
					}
					$opt = 1;
				} elsif ($cons ne "") {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					$propExpr .= " $word";
					$opt = 1;
				} else {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					if ($opt == 0) {
						$layer .= " $word";
					} else {
						$propExpr .= " $word";
					}
				}
			}
		}
		$outLine = &genOutLine("dfm_property_merge"," $layer $option $propExpr", $outLay, 2);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+property\s+net\s+(\S+)/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "dfm\\s+property\\s+net");
		my $word = "";
		my $option = "";
		my $opt = 0;
		my $propExpr = "";
		my $cons = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^intersecting/is) {
				$remain =~ s/^intersecting//is;
				$option .= " -intersecting";
				$opt = 1;
			} elsif ($remain =~ /^overlap/is) {
				$remain =~ s/^overlap//is;
				$option .= " -overlap";
				$opt = 1;
			} elsif ($remain =~ /^abut\s+also/is) {
				$remain =~ s/^abut\s+also//is;
				$option .= " -abut_also";
				$opt = 1;
			} elsif ($remain =~ /^singular/is) {
				$remain =~ s/^singular//is;
				$option .= " -single_point";
				$opt = 1;
			} elsif ($remain =~ /^multi/is) {
				$remain =~ s/^multi//is;
				$option .= " -multi";
				$opt = 1;
			} elsif ($remain =~ /^nomulti/is) {
				$remain =~ s/^nomulti//is;
				$option .= " -nomulti";
				$opt = 1;
			} elsif ($remain =~ /^nodal/is) {
				$remain =~ s/^nodal//is;
				$option .= " -nodal";
				$opt = 1;
			} elsif ($remain =~ /^corner/is) {
				$remain =~ s/^corner//is;
				$option .= " -corner";
				$opt = 1;
			} elsif ($remain =~ /^connected/is) {
				$remain =~ s/^connected//is;
				$option .= " -connected";
				$opt = 1;
			} elsif ($remain =~ /^not\s+connected/is) {
				$remain =~ s/^not\s+connected//is;
				$option .= " -not_connected";
				$opt = 1;
			} elsif ($remain =~ /^inside\s+of\s+\S+\s+\S+\s+\S+\s+\S+/is) {
				my ($x1, $y1, $x2, $y2);
				($x1, $y1, $x2, $y2, $remain) = 
					($remain =~ /^inside\s+of\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)(.*)$/is);
				$option .= " -inside_of $x1 $y1 $x2 $y2";
				$opt = 1;
			} elsif ($remain =~ /^split\s+all/is) {
				$remain =~ s/^split\s+all//is;
				$option .= " -split_all";
				$opt = 1;
			} elsif ($remain =~ /^split/is) {
				$remain =~ s/^split//is;
				$option .= " -split";
				$opt = 1;
			} elsif ($remain =~ /^accumulate\s+only/is) {
				$remain =~ s/^accumulate\s+only//is;
				$option .= " -accumulate_only";
				$opt = 1;
			} elsif ($remain =~ /^accumulate/is) {
				$remain =~ s/^accumulate//is;
				$option .= " -accumulate";
				$opt = 1;
			} elsif ($remain =~ /^by\s+net\s+only/is) {
				$remain =~ s/^by\s+net\s+only//is;
				$option .= " -by_net_only";
				$opt = 1;
			} elsif ($remain =~ /^by\s+net/is) {
				$remain =~ s/^by\s+net//is;
				$option .= " -by_net_only";
				$opt = 1;
			} elsif ($remain =~ /^nohier/is) {
				$remain =~ s/^nohier//is;
				$option .= " -no_hier";
				$opt = 1;
			} elsif ($remain =~ /^nopseudo/is) {
				$remain =~ s/^nopseudo//is;
				$option .= " -no_pseudo";
				$opt = 1;
			} elsif ($remain =~ /^by\s+cell\s+only/is) {
				$remain =~ s/^by\s+cell\s+only//is;
				$option .= " -by_cell_only";
				$opt = 1;
			} elsif ($remain =~ /^by\s+cell/is) {
				$remain =~ s/^by\s+cell//is;
				$option .= " -by_cell";
				$opt = 1;
			} else {
				if ($remain =~ /^\[[^\]]+\]/is) {
					($word, $remain) = ($remain =~ /^(\[[^\]]+\])(.*)$/is);
					$propExpr .= " \n$word";
					$opt = 1;
				} elsif ($remain =~ /^[<>=!]/) {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					my ($isOp, $resOp) = &transRelOp($word);
					if ($isOp > 0) {
						$propExpr .= " $resOp";
						if ($propExpr !~ /(gt|ge|lt|le|ne|eq)$/) {
							$propExpr .= "\n";
						}
						$cons = " $resOp";
					} else {
						print "!!CHK_ERR_DFM_NET<<$word>>!\n";
					}
					$opt = 1;
				} elsif ($cons ne "") {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					$propExpr .= " $word";
					$opt = 1;
				} else {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					if ($opt == 0) {
						$layer .= " $word";
					} else {
						$propExpr .= " $word";
					}
				}
			}
		}
		$outLine = &genOutLine("dfm_property_net"," $layer $option $propExpr", $outLay, 2);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+property\s+(\S+)/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "dfm\\s+property");
		my $word = "";
		my $option = "";
		my $opt = 0;
		my $propExpr = "";
		my $cons = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^intersecting/is) {
				$remain =~ s/^intersecting//is;
				$option .= " -intersecting";
				$opt = 1;
			} elsif ($remain =~ /^overlap/is) {
				$remain =~ s/^overlap//is;
				$option .= " -overlap";
				$opt = 1;
			} elsif ($remain =~ /^abut\s+also/is) {
				$remain =~ s/^abut\s+also//is;
				$option .= " -abut_also";
				$opt = 1;
			} elsif ($remain =~ /^singular/is) {
				$remain =~ s/^singular//is;
				$option .= " -single_point";
				$opt = 1;
			} elsif ($remain =~ /^multi/is) {
				$remain =~ s/^multi//is;
				$option .= " -multi";
				$opt = 1;
			} elsif ($remain =~ /^nomulti/is) {
				$remain =~ s/^nomulti//is;
				$option .= " -nomulti";
				$opt = 1;
			} elsif ($remain =~ /^nodal/is) {
				$remain =~ s/^nodal//is;
				$option .= " -nodal";
				$opt = 1;
			} elsif ($remain =~ /^corner/is) {
				$remain =~ s/^corner//is;
				$option .= " -corner";
				$opt = 1;
			} elsif ($remain =~ /^connected/is) {
				$remain =~ s/^connected//is;
				$option .= " -connected";
				$opt = 1;
			} elsif ($remain =~ /^not\s+connected/is) {
				$remain =~ s/^not\s+connected//is;
				$option .= " -not_connected";
				$opt = 1;
			} elsif ($remain =~ /^inside\s+of\s+\S+\s+\S+\s+\S+\s+\S+/is) {
				my ($x1, $y1, $x2, $y2);
				($x1, $y1, $x2, $y2, $remain) = 
					($remain =~ /^inside\s+of\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)(.*)$/is);
				$option .= " -inside_of $x1 $y1 $x2 $y2";
				$opt = 1;
			} elsif ($remain =~ /^split\s+all/is) {
				$remain =~ s/^split\s+all//is;
				$option .= " -split_all";
				$opt = 1;
			} elsif ($remain =~ /^split/is) {
				$remain =~ s/^split//is;
				$option .= " -split";
				$opt = 1;
			} elsif ($remain =~ /^accumulate\s+only/is) {
				$remain =~ s/^accumulate\s+only//is;
				$option .= " -accumulate_only";
				$opt = 1;
			} elsif ($remain =~ /^accumulate/is) {
				$remain =~ s/^accumulate//is;
				$option .= " -accumulate";
				$opt = 1;
			} elsif ($remain =~ /^by\s+net\s+only/is) {
				$remain =~ s/^by\s+net\s+only//is;
				$option .= " -by_net_only";
				$opt = 1;
			} elsif ($remain =~ /^by\s+net/is) {
				$remain =~ s/^by\s+net//is;
				$option .= " -by_net_only";
				$opt = 1;
			} elsif ($remain =~ /^nohier/is) {
				$remain =~ s/^nohier//is;
				$option .= " -no_hier";
				$opt = 1;
			} elsif ($remain =~ /^nopseudo/is) {
				$remain =~ s/^nopseudo//is;
				$option .= " -no_pseudo";
				$opt = 1;
			} elsif ($remain =~ /^by\s+cell\s+only/is) {
				$remain =~ s/^by\s+cell\s+only//is;
				$option .= " -by_cell_only";
				$opt = 1;
			} elsif ($remain =~ /^by\s+cell/is) {
				$remain =~ s/^by\s+cell//is;
				$option .= " -by_cell";
				$opt = 1;
			} else {
				if ($remain =~ /^\[[^\]]+\]/is) {
					($word, $remain) = ($remain =~ /^(\[[^\]]+\])(.*)$/is);
					$propExpr .= " $word";
					$opt = 1;
				} elsif ($remain =~ /^[<>=!]/) {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					my ($isOp, $resOp) = &transRelOp($word);
					if ($isOp > 0) {
						$propExpr .= " $resOp";
						if ($propExpr !~ /(gt|ge|lt|le|ne|eq)$/) {
							$propExpr .= "\n";
						}
						$cons = " $resOp";
					} else {
						print "!!CHK_ERR<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
					}
					$opt = 1;
				} elsif ($cons ne "") {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					$propExpr .= " $word";
					$opt = 1;
				} else {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					if ($opt == 0) {
						$layer .= " $word";
					} else {
						$propExpr .= " $word";
					}
				}
			}
		}
		$outLine = &genOutLine("dfm_property"," $layer $option $propExpr", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+text\s+/is) {
		my ($outLay, $layer, $remain) =
				&parseCmdWithLayer($inLine, "dfm\\s+text");
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^primary\s+only\b/is) {
				$remain =~ s/^primary\s+only\b//is;
				$option .= " -primary_only";
			} elsif ($remain =~ /^property\s+string\b/is) {
				$remain =~ s/^property\s+string//is;
				$option .= " -string";
			} elsif ($remain =~ /^property\s+invalid\s+number\b/is) {
				$remain =~ s/^property\s+invalid\s+number//is;
				$option .= " -invalid_number";
			} elsif ($remain =~ /^property\s+number\b/is) {
				$remain =~ s/^property\s+number//is;
				$option .= " -number";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				if ($remain !~ /\S+/) {
					$option .= " -property_name $word";
				} else {
					$option .= " $word";
				}
			}
		}
		$outLine = &genOutLine("dfm_text"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdfm\s+rdb\s+/is) {
		my ($outLay, $layer, $remain) = 
				&parseCmdWithLayer($inLine, "dfm\\s+rdb");
		my $haveLayer = 1;
		if ($layer =~ /^gds$/is) {
			$layer = "-gds";
			$haveLayer = 0;	
		} elsif ($layer =~ /^oasis$/is) {
			$layer = "-oasis";
			$haveLayer = 0;	
		}
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/is) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^nodal\s+all\b/is) {
				$remain =~ s/^nodal\s+all//is;
				$option .= " -nodal all";
			} elsif ($remain =~ /^nodal\s+other\b/is) {
				$remain =~ s/^nodal\s+other//is;
				$option .= " -nodal other";
			} elsif ($remain =~ /^nodal\b/is) {
				$remain =~ s/^nodal//is;
				$option .= " -nodal";
			} elsif ($remain =~ /^nopseudo\b/is) {
				$remain =~ s/^nopseudo//is;
				$option .= " -nopseudo";
			} elsif ($remain =~ /^noempty\b/is) {
				$remain =~ s/^noempty//is;
				$option .= " -noempty";
			} elsif ($remain =~ /^output\b/is) {
				$remain =~ s/^output//is;
				$option .= " -output";
			} elsif ($remain =~ /^maximum\s+all\b/is) {
				$remain =~ s/^maximum\s+all//is;
				$option .= " -maximum all";
			} elsif ($remain =~ /^maximum\s+\S+/is) {
				($word, $remain) = ($remain =~ /^maximum\s+(\S+)(.*)/is);
				$option .= " -maximum $word";
			} elsif ($remain =~ /^joined\b/is) {
				$remain =~ s/^joined//is;
				$option .= " -joined";
			} elsif ($remain =~ /^all\s+cells\b/is) {
				$remain =~ s/^all\s+cells//is;
				$option .= " -all_cells";
			} elsif ($remain =~ /^same\s+cells\b/is) {
				$remain =~ s/^same\s+cells//is;
				$option .= " -same_cells";
			} elsif ($remain =~ /^transition\b/is) {
				$remain =~ s/^transition//is;
				$option .= " -transition";
			} elsif ($remain =~ /^abut\s+also\b/is) {
				$remain =~ s/^abut\s+also//is;
				$option .= " -abut_also";
			} elsif ($remain =~ /^result\s+cells\b/is) {
				$remain =~ s/^result\s+cells//is;
				$option .= " -result_cells";
			} elsif ($remain =~ /^checkname\b/is) {
				$remain =~ s/^checkname//is;
				$option .= " -checkname";
			} elsif ($remain =~ /^comment\b/is) {
				$remain =~ s/^comment//is;
				$option .= " -comment";
			} elsif ($remain =~ /^"[^"]+"/is) {
				($word, $remain) = ($remain =~ /^("[^"]+")(.*)$/is);
				if (length($word) > 20) {
					$option .= " $word\n";
				} else {
					$option .= " $word";
				}
			} elsif ($remain =~ /^annotate\b/is) {
				$remain =~ s/^annotate//is;
				$option .= " -annotate";
			} elsif ($remain =~ /^\[[^\]]+\]/is) {
				($word, $remain) = ($remain =~ /^(\[[^\]]+\])(.*)$/is);
				$option .= " $word";
			} elsif ($remain =~ /^cell\s+space\b/is) {
				$remain =~ s/^cell\s+space//is;
				$option .= " -cell_space";
			} elsif ($remain =~ /^NULL\b/is) {
				$remain =~ s/^null//is;
				$option .= " -null";
			} elsif ($remain =~ /^CSG\b/is) {
				$remain =~ s/^csg//is;
				$option .= " -csg";
			} elsif ($remain =~ /^usermerged\b/is) {
				$remain =~ s/^usermerged//is;
				$option .= " -user_merged";
			} elsif ($remain =~ /^pseudo\b/is) {
				$remain =~ s/^pseudo//is;
				$option .= " -pseudo";
			} elsif ($remain =~ /^user\b/is) {
				$remain =~ s/^user//is;
				$option .= " -user";
			} elsif ($remain =~ /^keepempty\b/is) {
				$remain =~ s/^keepempty//is;
				$option .= " -keep_empty";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("dfm_rdb"," $layer $option", $outLay, 1);
		return ($outLine, 0);	
	} elsif ($inLine =~ /^\s*dfm\s+function\s+/is) {
		my ($content) = ($inLine =~ /^\s*dfm\s+function\s+(\[.*)$/is);
		$outLine = "dfm_function $content;\n";
		return ($outLine, 0); 
	} elsif ($inLine =~ /^\s*tvf\s+function\s+\S+/is) {
		my ($funcName, $content) = ($inLine =~ /^\s*tvf\s+function\s+([^\[]+)(\[.*)$/is);
		$outLine = "tvf_function $funcName $content;\n";
		return ($outLine, 0); 
	} elsif ($inLine =~ /^\s*text\s+depth\s+/is) {
		my ($textDepth) = ($inLine =~ /^\s*text\s+depth\s+(\S+)/is);
		if ($textDepth =~ /primary/is) {
			$textDepth = "-primary";
		} elsif ($textDepth =~ /all/is) {
			$textDepth = "-all";
		}
		$outLine = "text_depth $textDepth;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /net\s+area\s+ratio\s+\S+/is) {
		my $outLay = "";
		my $remain = "";
		my $preLay = "";
		if ($inLine =~ /=\s+net\s+area\s+ratio\b/is) {
			$outLay = &extractOutLayer($inLine, \$remain);
			$remain =~ s/^\s*net\s+area\s+ratio\s+//is;
		} elsif ($inLine =~ /=\s+(\S+)\s+net\s+area\s+ratio\b/is) {
			$outLay = &extractOutLayer($inLine, \$remain);
			$remain =~ s/\s+net\s+area\s+ratio\s+/ /is;
		} else {
			if ($inLine =~ /^\s*net\s+area\s+ratio\s+(.*)$/is) {
				($remain) = ($inLine =~ /\s*net\s+area\s+ratio\s+(.*)$/is);
			} else {
				($preLay, $remain) = ($inLine =~ /^(.*)\s+net\s+area\s+ratio\s+(.*)$/is);
				$remain = "$preLay $remain";
			}
		}
		my $ps = 0; # for layer
		my $layers = "";
		my $cons = "";
		my $expression = "";
		my $option = "";
		my $word = "";
		my $needConsValue = 0;
		my $isAccu = 0;
		my $accumSet = "";
		if ($remain =~ /\S+\[/) {
			$remain =~ s/(\S+)\[/$1 \[/;
		}
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;	
			if ($ps == 0) {
				if ($remain =~ /^[><=!]/) {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					my ($isOp, $resOp) = &transRelOp($word);	
					$cons .= $resOp;
					$ps = 1;
				} elsif ($remain =~ /^\[.*\]/) {
					($word, $remain) = ($remain =~ /^(\[[^\]]+\])\s*(.*)$/is);
					$expression = $word;
					$ps = 1;
				} elsif ($remain =~ /^[a-zA-Z\$0-9_\.:]+/) {
					($word, $remain) = ($remain =~ /^([a-zA-Z0-9\$_\.:]+)(.*)$/is);
					$layers .= " $word";
				}
			} elsif ($ps == 1) {
				if ($remain =~ /^[><=!]/) {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					my ($isOp, $resOp) = &transRelOp($word);	
					$cons .= $resOp;
					if ($word !~ /[^!<>=]/) {
						$needConsValue = 1;
					}
					$ps = 2;
					$isAccu = 0;	
				} elsif ($remain =~ /^\[.*\]/is) {
					($word, $remain) = ($remain =~ /^(\[[^\]]+\])\s*(.*)$/is);
					$expression = $word;
					$ps = 2;
					$isAccu = 0;	
				} elsif ($remain =~ /^\[/) {
					$outLine = "antenna $layers $cons";
					$isAccu = 0;	
					return ($outLine, 1); # bracket parenthesis incomplete
				} elsif ($remain =~ /^accumulate\b/is) {
					$remain =~ s/^accumulate//is;
					$accumSet .= " -accumulate";
					$isAccu = 1;	
				} else {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					if ($isAccu == 1) {
						$accumSet .= " $word";
					} else {
						$cons .= " $word";
					}
				}
			} elsif ($ps == 2) {
				if ($remain =~ /^RDB/is) {
					$remain =~ s/^RDB\s+//is;
					$option .= " -rdb $remain";
					$remain = "";
					$isAccu = 0;	
				} elsif ($remain =~ /^accumulate\b/is) {
					$remain =~ s/^accumulate//is;
					$option .= " -accumulate";
					$isAccu = 1;	
				} else {
					($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
					if ($needConsValue == 1) {
						$cons .= " $word";
					} elsif ($isAccu == 1) {
						$accumSet .= " $word";
					} else {
						$option .= " $word";
					}
				}
			}
		}
		my $currKey = "antenna";
		my $outLine = " $layers $cons";
		if ($accumSet =~ /\S+/) {
			$outLine .= " $accumSet";
		} 
		if ($expression !~ /^\s*$/) {
			$expression =~ s/^\s*\[/"/;
			$expression =~ s/\]\s*$/"/;
			$outLine .= "\n\t-expr $expression";
		}
		if ($option !~ /^\s*$/) {
			$outLine .= "\n\t$option";
		}
		$outLine = &genOutLine($currKey, $outLine, $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bnet\s+area\s+\S+/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "net\\s+area");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^[><=!]/) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				my ($isOp, $resOp) = &transRelOp($word);	
				if ($isOp > 0) {
					$option .= $resOp;
				}
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}	
		}
		$outLine = &genOutLine("net_area"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*group\s+\S+/is) {
		my ($groupName, $remain) = ($inLine =~ /^\s*group\s+(\S+)\s+(.*)$/is);
		$outLine = "group_rules $groupName $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*text\s+layer\s+(\S+)\s+(\S+)/is) {
		my ($layId, $remain) = ($inLine =~ /^\s*text\s+layer\s+(\S+)\s+(.*)$/is);
		my $allLays = $layId;
		my $attach = "";
		if ($remain =~ /attach/is) {
			my ($lays, $a) = ($remain =~ /^(.*)attach\s+(.*)$/is);
			$allLays = "$layId ".$lays;
			$attach = $a;
			$attach =~ s/\s*//is;
		} else {
			$allLays = "$allLays ".$remain;
		}
		$outLine = "text_layer $allLays;\n";
		if ($attach ne "") {
			$outLine = $outLine."attach $attach;\n\n";
		}
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*text\s+layer\s+(\S+)\s*$/is) {
		my ($layId) = ($inLine =~ /^\s*text\s+layer\s+(\S+)/is);
		$outLine = "text_layer $layId;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*attach\s+\S+/is) {
		my ($content) = ($inLine =~ /^\s*attach\s+(.*)$/is);
		$outLine = "attach $content;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+case/is) {
		$outLine = "// no match for $inLine";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*flag\s*offgrid\s/is) {
		my ($flag) = ($inLine =~ /^\s*flag\s+offgrid\s+(\S+)/is);
		$flag = lc $flag;
		$outLine = "flag -offgrid $flag;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*flag\s*acute\s/is) {
		my ($flag) = ($inLine =~ /^\s*flag\s+acute\s+(\S+)/is);
		$flag = lc $flag;
		$outLine = "flag -acute $flag;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*flag\s*skew\s/is) {
		my ($flag) = ($inLine =~ /^\s*flag\s+skew\s+(\S+)/is);
		$flag = lc $flag;
		$outLine = "flag -skew $flag;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*flag\s*nonsimple\s+path\s+/is) {
		my ($flag) = ($inLine =~ /^\s*flag\s+nonsimple\s+path\s+(\S+)/is);
		$flag = lc $flag;
		$outLine = "flag -nonsimple_path $flag;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*flag\s*nonsimple\s/is) {
		my ($flag) = ($inLine =~ /^\s*flag\s+nonsimple\s+(\S+)/is);
		$flag = lc $flag;
		$outLine = "flag -nonsimple $flag;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*exclude\s+cell\s+(.*)$/is) {
		my ($cells) = ($inLine =~ /^\s*exclude\s+cell\s+(.*)$/is);
		$outLine = "exclude_cell $cells;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+top\s+layer\s+(.*)$/is) {
		my ($layers) = ($inLine =~ /^\s*layout\s+top\s+layer\s+(.*)$/is); 
		$outLine = "top_layer $layers;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*layout\s+base\s+layer\s+(.*)$/is) {
		my ($layers) = ($inLine =~ /^\s*layout\s+base\s+layer\s+(.*)$/is); 
		$outLine = "base_layer $layers;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*exclude\s+skew\s+\S+/is) {
		my ($layers) = ($inLine =~ /^\s*exclude\s+skew\s+(.*)$/is); 
		$outLine = "exclude_skew $layers;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*exclude\s+acute\s+\S+/is) {
		my ($layers) = ($inLine =~ /^\s*exclude\s+acute\s+(.*)$/is); 
		$outLine = "exclude_acute $layers;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*exclude\s+offgrid\s+\S+/is) {
		my ($layers) = ($inLine =~ /^\s*exclude\s+offgrid\s+(.*)$/is); 
		$outLine = "exclude_offgrid $layers;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*exclude\s+angled\s+\S+/is) {
		my ($layers) = ($inLine =~ /^\s*exclude\s+angled\s+(.*)$/is); 
		$outLine = "exclude_angled $layers;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\bORNET\b\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "ornet");
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^BY\s+NET/is) {
				$remain =~ s/^BY\s+NET//is;
				$option .= " -by_net";
			} elsif ($remain =~ /^BY\s+SHAPE/is) {
				$remain =~ s/^BY\s+SHAPE//is;
				$option .= " -by_shape";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("ornet"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*variable\s+\S+/is) {
		my ($variable, $remain) = ($inLine =~ /^\s*variable\s+(\S+)\s+(.*)$/is);
		$outLine = "variable $variable $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /(^|\s+|=)DENSITY\s+/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "density");
		my $option = "";
		my $expr = "";
		my $attrCenter = "";
		while ($remain !~ /^\s*$/) {
			$remain =~ s/^\s*//;
			my $word = "";
			if ($remain =~ /^INSIDE\s+OF\s+EXTENT\s+/is) {
				$option .= " -inside_of extent";
				$remain =~ s/^INSIDE\s+OF\s+EXTENT\s+//is;
			} elsif ($remain =~ /^INSIDE\s+OF\s+LAYER\s+\S+/is) {
				my ($inside) = ($remain =~ /^INSIDE\s+OF\s+LAYER\s+(\S+)\s*/is);
				$remain =~ s/^INSIDE\s+OF\s+LAYER\s+\S+\s*//is;
				$option .= " -inside_of layer $inside";
			} elsif ($remain =~ /^PRINT\s+(\S+)/is) {
				my $printName = "";
				($printName, $remain) = ($remain =~ /^PRINT\s+(\S+)(.*)$/is);
				$option .= " -print $printName";
			} elsif ($remain =~ /^[><=!]/) {
				($word, $remain) = ($remain =~ /^([><=!]+)(.*)$/is);
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";	
				}
			} elsif ($remain =~ /^WINDOW\s+(\S+)/is) {
				($word, $remain) = ($remain =~ /^WINDOW\s+(\S+)(.*)$/is);
				$option .= " -window $word";
			} elsif ($remain =~ /^STEP\s+(\S+)/is) {
				($word, $remain) = ($remain =~ /^STEP\s+(\S+)(.*)$/is);
				$option .= " -step $word";
			} elsif ($remain =~ /^GRADIENT\s+/is) {
				($word, $remain) = ($remain =~ /^(GRADIENT)\s+(.*)$/is);
				$option .= " -gradient";
				$attrCenter = "gradient";
			} elsif ($remain =~ /^absolute\b/is) {
				$remain =~ s/^absolute\b//is;
				$option .= " -absolute";
			} elsif ($remain =~ /^relative\b/is) {
				$remain =~ s/^relative\b//is;
				$option =~ s/($attrCenter.*?)$/normalized_$1/is;
			} elsif ($remain =~ /^magnitude\s+/is) {
				($word, $remain) = ($remain =~ /^(magnitude)\s+(.*)$/is);
				$option .= " -magnitude";
				$attrCenter = "magnitude";
			} elsif ($remain =~ /^corner\b/is) {
				$remain =~ s/^corner\b//is;
				$option .= " -corner";
			} elsif ($remain =~ /^backup/is) {
				($remain) = ($remain =~ /^backup(.*)$/is);
				$option .= " -backup";
			} elsif ($remain =~ /^\[[^\]]+\]/is) {
				($expr, $remain) = ($remain =~ /^(\[[^\]]+\])(.*)$/is);
				$option .= " -expr $expr";
			} elsif ($remain =~ /^RDB\s+ONLY\s+\S+/is) {
				($word, $remain) = ($remain =~ /^RDB\s+ONLY\s+(\S+)(.*)$/is);
				$option .= " -rdb_only $word";
			} elsif ($remain =~ /^RDB\s+\S+/is) {
				($word, $remain) = ($remain =~ /^RDB\s+(\S+)(.*)$/is);
				$option .= " -rdb $word";
			} elsif ($remain =~ /(\S+).*$/is) {
				($word, $remain) = ($remain =~ /(\S+)(.*)$/is);
				$option .= " $word";
			} else {
				$word = $remain;
				$word =~ s/\s+//g;
				$remain = "";
			}
		}
		$outLine = &genOutLine("density"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bnet\s+interact\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = &parseCmdWithNotLayer($inLine, "net\\s+interact");
		my $option = "";
		my $value = "";
		while ($remain =~ /^\S+/) {
			$remain =~ s/^\s*//;
			my $word = "";
			($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
			$remain =~ s/^\s*//;
			if ($word =~ /^\w/ and $word =~ /[<>=!]/) {
				my ($w1, $o1) = ($word =~ /^([^<>=!]+)([<>=!].*)/is);
				$word = $w1;
				$remain = $o1.$remain;
			}
			if ($word =~ /^[<>=!]/) {
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					print "!!CHK_ERR<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} else {
				$option .= " $word";
			}	
		}
		$outLine = &genOutLine("net_interact"," $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bshrink\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "shrink");
		my $option = "";
		my $value = "";
		while ($remain !~ /^\s*$/) {
			my $word = "";
			$remain =~ s/^\s*//;
			if ($remain =~ /^TOP\s+BY\s+\S+/is) {
				($value, $remain) = ($remain=~ /^TOP\s+BY\s+(\S+)(.*)$/is);
				$option .= " -top $value";
			} elsif ($remain =~ /^BOTTOM\s+BY\s+\S+/is) {
				($value, $remain) = ($remain=~ /^bottom\s+BY\s+(\S+)(.*)$/is);
				$option .= " -bottom $value";
			} elsif ($remain =~ /^left\s+BY\s+\S+/is) {
				($value, $remain) = ($remain=~ /^left\s+BY\s+(\S+)(.*)$/is);
				$option .= " -left $value";
			} elsif ($remain =~ /^right\s+BY\s+\S+/is) {
				($value, $remain) = ($remain=~ /^right\s+BY\s+(\S+)(.*)$/is);
				$option .= " -right $value";
			} elsif ($remain =~ /^sequential/is) {
				$remain =~ s/^sequential//;
				$option .= " -sequential";
			}
		}
		$outLine = &genOutLine("shrink"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bgrow\s+\S+/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "grow");

		my $option = "";
		my $value = "";
		while ($remain !~ /^\s*$/) {
			my $word = "";
			$remain =~ s/^\s*//;
			if ($remain =~ /^TOP\s+BY\s+\S+/is) {
				($value, $remain) = ($remain=~ /^TOP\s+BY\s+(\S+)(.*)$/is);
				$option .= " -top $value";
			} elsif ($remain =~ /^BOTTOM\s+BY\s+\S+/is) {
				($value, $remain) = ($remain=~ /^bottom\s+BY\s+(\S+)(.*)$/is);
				$option .= " -bottom $value";
			} elsif ($remain =~ /^left\s+BY\s+\S+/is) {
				($value, $remain) = ($remain=~ /^left\s+BY\s+(\S+)(.*)$/is);
				$option .= " -left $value";
			} elsif ($remain =~ /^right\s+BY\s+\S+/is) {
				($value, $remain) = ($remain=~ /^right\s+BY\s+(\S+)(.*)$/is);
				$option .= " -right $value";
			} elsif ($remain =~ /^\s*sequential\b/is) {
				$remain =~ s/^\s*sequential\b//is;
				$option .= " -sequential";
			} else {
				($value, $remain) = ($remain =~ /^\s*(\S+)(.*)$/is);
				$option .= " $value"; 
			}
		}
		$outLine = &genOutLine("grow"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdrawn\s+offgrid/is) {
		$outLine = "layout_input -offgrid;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdrawn\s+skew/is) {
		$outLine = "layout_input -skew;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdrawn\s+acute/is) {
		$outLine = "layout_input -acute;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\boffgrid\s+directional/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "offgrid\\s+directional");
		my $option = "";
		while ($remain !~ /^\s*$/) {
			$remain =~ s/^\s*//;
			my $word = "";
			my $key = "";
			if ($remain =~ /^INSIDE\s+OF\s+LAYER\s+\S+/is) {
				my ($inside) = ($remain =~ /^INSIDE\s+OF\s+LAYER\s+(\S+)\s*/is);
				$remain =~ s/^INSIDE\s+OF\s+LAYER\s+\S+\s*//is;
				$option .= " -inside_of_layer $inside";
			} elsif ($remain =~ /^top\s*/is) {
				($key, $remain) = ($remain =~ /^(top\s*)(.*)$/is);
				$option .= " -top ";
			} elsif ($remain =~ /^bottom\s*/is) {
				($key, $remain) = ($remain =~ /^(bottom\s*)(.*)$/is);
				$option .= " -bottom ";
			} elsif ($remain =~ /^left\s*/is) {
				($key, $remain) = ($remain =~ /^(left\s*)(.*)$/is);
				$option .= " -left ";
			} elsif ($remain =~ /^right\s*/is) {
				($key, $remain) = ($remain =~ /^(right\s*)(.*)$/is);
				$option .= " -right ";
			} elsif ($remain =~ /^face\s*/is) {
				($key, $remain) = ($remain =~ /^(face\s*)(.*)$/is);
				$option .= " -face ";
			} elsif ($remain =~ /^noface\s*/is) {
				($key, $remain) = ($remain =~ /^(noface\s*)(.*)$/is);
				$option .= " -noface ";
			} elsif ($remain =~ /^edge\s*/is) {
				($key, $remain) = ($remain =~ /^(edge\s*)(.*)$/is);
				$option .= " -edge ";
			} elsif ($remain =~ /(\S+).*$/is) {
				($word, $remain) = ($remain =~ /(\S+)(.*)$/is);
				$option .= " $word";
			} else {
				$word = $remain;
				$word =~ s/\s+//g;
				$remain = "";
			}
		}
		$outLine = &genOutLine("offgrid_directional"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\boffgrid\s+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "offgrid");
		my $option = "";
		$outLine = &genOutLine("offgrid"," $layer $remain", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bwith\s+neighbor\s+/is) {
		my ($outLay, $notFlag, $layers, $remain) = 
			&parseCmdWithNotLayer($inLine, "with\\s+neighbor");
		my $word = "";
		my $cons1 = "";
		my $cons2 = "";
		my $option = "";
		my $haveSpace = 0;
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^[<>=!]/) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					if ($haveSpace == 0) {
						$cons1 = $resOp;
					} else {
						$cons2 = $resOp;
					}
				} else {
					print "!!CHK_PARSE<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} elsif ($remain =~ /^SPACE\s+/is) {
				$remain =~ s/^SPACE//is;
				$haveSpace = 1;
			} elsif ($remain =~ /^square\b/is) {
				$remain =~ s/^square//is;
				$option .= " -metric square";
			} elsif ($remain =~ /^centers\b/is) {
				$remain =~ s/^centers//is;
				$option .= " -centers";
			} elsif ($remain =~ /^octagonal\s+only\b/is) {
				$remain =~ s/^octagonal\s+only//is;
				$option .= " -octagonal_only";
			} elsif ($remain =~ /^orthogonal\s+only\b/is) {
				$remain =~ s/^orthgonal\s+only//is;
				$option .= " -orthogonal_only";
			} elsif ($remain =~ /^inside\s+of\s+layer\s+\S+/is) {
				($word, $remain) = ($remain =~ /^inside\s+of\s+layer\s+(\S+)(.*)$/is);
				$option .= " -inside_of layer $word";
			} elsif ($remain =~ /^not\s+connected\b/is) {
				$remain =~ s/^not\s+connected//is;
				$option .= " -not_connected";
			} elsif ($remain =~ /^connected\b/is) {
				$remain =~ s/^connected//is;
				$option .= " -connected";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				if ($haveSpace == 1) {
					$cons2 .= " $word";
				} elsif ($cons1 ne "") {
					$cons1 .= " $word";
				} else {
					$layers .= " $word";
				}
			}
		}
		$outLine = &genOutLine("select"," -with_neighbor $notFlag $layers $cons1 -space $cons2 $option", $outLay, 1);
		return ($outLine, 0);	
	} elsif ($inLine =~ /\bwith\s+width\s+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "with\\s+width");
		my $option = "";
		my $value = "";
		while ($remain !~ /^\s*$/) {
			my $word = "";
			if ($remain =~ /(\S+)\s+(\S+)\s*$/) {
				($word, $remain) = ($remain =~ /(\S+)\s+(.*)$/);
			} else {
				$word = $remain;
				$word =~ s/\s+//g;
				$remain = "";
			}
			my ($isOp, $resOp) = &transRelOp($word);
			if ($isOp > 0) {
				$option .= " $resOp";
			} else {
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("with_width"," $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\s*perimeter\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "perimeter");
		my $option = "";
		my $word = "";
		while ($remain !~ /^\s*$/) {
			$remain =~ s/^\s+//;
			if ($remain =~ /(\S+)\s+(\S+)$/) {
				($word, $remain) = ($remain =~ /(\S+)\s+(.*)$/);
			} else {
				$word = $remain;
				$word =~ s/\s+//g;
				$remain = "";
			}
			my ($isOp, $resOp) = &transRelOp($word);
			if ($isOp > 0) {
				$option .= " $resOp";
			} else {
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("perimeter"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bvertex\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "vertex");
		my $option = "";
		my $word = "";
		while ($remain !~ /^\s*$/) {
			$remain =~ s/^\s+//;
			if ($remain =~ /(\S+)\s+(\S+)$/) {
				($word, $remain) = ($remain =~ /(\S+)\s+(.*)$/);
			} else {
				$word = $remain;
				$word =~ s/\s+//g;
				$remain = "";
			}
			my ($isOp, $resOp) = &transRelOp($word);
			if ($isOp > 0) {
				$option .= " $resOp";
			} else {
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("vertex"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bextent\s+cell\b/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "extent\\s+cell");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^ORIGINAL/is) {
				$remain =~ s/^original//is;
				$option .= " -original";
			} elsif ($remain =~ /^mapped/is) {
				$remain =~ s/^mapped//is;
				$option .= " -mapped";
			} elsif ($remain =~ /^occupied/is) {
				$remain =~ s/^occupied//is;
				$option .= " -occupied";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("extent_cell"," $layer $option", $outLay, 2);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bextents\s+\S+/is and $inLine !~ /region\s+extents/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "extents");
		my $byLayer = "";
		my $option = "";
		while ($remain =~  /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^CENTERS\b/is) {
				($remain) = ($remain =~ /^CENTERS(.*)$/is);
				$option .= " -centers";
			} elsif ($remain =~ /^\S+/) {
				($byLayer, $remain) = ($remain =~ /(\S+)(.*)$/);
				$option .= " $byLayer";
			}
		}
		$outLine = &genOutLine("bbox"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bconvex\s+edge\s+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "convex\\s+edge");
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^with\s+length/is) {
				$remain =~ s/^with\s+length//is;
				$option .= " -with_length";
			} elsif ($remain =~ /^angle1/is) {
				$remain =~ s/^angle1//is;
				$option .= " -angle1";
			} elsif ($remain =~ /^angle2/is) {
				$remain =~ s/^angle2//is;
				$option .= " -angle2";
			} elsif ($remain =~ /^length1/is) {
				$remain =~ s/^length1//is;
				$option .= " -length1";
			} elsif ($remain =~ /^length2/is) {
				$remain =~ s/^length2//is;
				$option .= " -length2";
			} elsif ($remain =~ /^[<>=!]/) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					$option .= " !!!>>$word<<<!!!";
				}
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		my $outLine = &genOutLine("convex_edge"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bdonut\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "donut");
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//is;
			my $word = "";
			($word, $remain) = ($remain =~ /^(\S+)(.*)$/);	
			if ($remain =~ /^[<>=!]/) {
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					$option .= " $word";
				}
			} else {
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("select"," -donut $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\brectangles\s+\S+/is) {
		my ($outLay, $opr, $remain) = 
			&parseCmdWithLayer($inLine, "rectangles");
		$remain = $opr." $remain";
		my $option = "";
		my $arIndex = 0;
		my $optBegin = 0;
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^offset\s+/is) {
				$option .= " -offset";
				$remain =~ s/^offset\b//is;
				$optBegin = 1;
			} elsif ($remain =~ /^inside\s+of\s+layer\s+/is) {
				$option .= " -inside_of layer";	
				$remain =~ s/^inside\s+of\s+layer\b//is;
				$optBegin = 1;
			} elsif ($remain =~ /^inside\s+of\s+/is) {
				$option .= " -inside_of";
				$remain =~ s/^inside\s+of\s+//is;
				$optBegin = 1;
			} elsif ($remain =~ /^maintain\s+spacing\b/is) {
				$option .= " -maintain_spacing";
				$remain =~ s/^maintain\s+spacing//is;
				$optBegin = 1;
			} else {
				my $word;
				($word, $remain) = ($remain =~ /(\S+)(.*)$/is);
				if ($optBegin == 0) {
					if ($arIndex == 0) {
						$option .= " -width";
						$arIndex = 1;
					} elsif ($arIndex == 1) {
						$option .= " -length";
						$arIndex = 2;
					} elsif ($arIndex == 2) {
						$option .= " -spacing";
						$arIndex = 3;
					}
				}
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("rect_gen"," $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\brectangle\s+(enc|enclosure)\s+\S+\s+\S+/is) {
		my ($preKey, $key,$layer1, $layer2, $remain) = ($inLine=~ /^\s*(.*)rectangle\s+(enc|enclosure)\s+(\S+)\s+(\S+)(.*)$/is);
		my $outLay = "";
		my $aftE = "";
		my $tail = "";
		if ($preKey =~ /=/) {
			$outLay = &extractOutLayer($preKey, \$aftE);
		} else {
			$aftE = $preKey;
		}
		if ($aftE =~ /\S+/) {
			print "!!CHK_ERR_AFT_RECT<<$aftE>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
		}
		my $word = "";
		my $inAbut = 0;
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^ABUT/is) {
				$remain =~ s/^ABUT//is;
				$option .= " -abut";
			} elsif ($remain =~ /^SINGULAR/is) {
				$remain =~ s/^singular//is;
				$option .= " -single_point";
			} elsif ($remain =~ /^outside\s+also\b/is) {
				$remain =~ s/^outside\s+also\b//is;
				$option .= " -outside_also";
			} elsif ($remain =~ /^orthongonal\s+only\b/is) {
				$remain =~ s/^orthongonal\s+only\b//is;
				$option .= " -orthongonal_only";
			} elsif ($remain =~ /^[<>=!]/) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				my ($isOp, $resOp) = &transAbutRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					print "!!CHK_ERR<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} elsif ($remain =~ /^good\b/is) {
				$remain =~ s/^good\b//is;
				$option .= " -good";
			} elsif ($remain =~ /^bad\b/is) {
				$remain =~ s/^bad\b//is;
				$option .= " -bad";
			} elsif ($remain =~ /^square/is) {
				$remain =~ s/^square//is;
				$option .= " square";
			} elsif ($remain =~ /^opposite\s+extented/is) {
				$remain =~ s/^opposite\s+extented//is;
				$option .= " opposite_extented";
			} elsif ($remain =~ /^opposite\s+symmetric/is) {
				$remain =~ s/^opposite\s+symmetric//is;
				$option .= " opposite_symmetric";
			} elsif ($remain =~ /^opposite/is) {
				$remain =~ s/^opposite//is;
				$option .= " opposite";
			} elsif (&isKeywords($remain)) {
				$tail = $remain;
				$remain = "";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}	
		}
		$outLine = &genOutLine("rect_enc"," $option $layer1 $layer2", $outLay, 1);
		if ($tail =~ /\S+/) {
			$outLine .= "\n\t$tail;\n";
		}
		return ($outLine, 0);	
	} elsif ($inLine =~ /\b(coincident|coin)\s+(inside|in)\s+edge\s+/is) {
		$inLine =~ s/\b(coincident|coin)\s+(inside|in)\s+edge\s+/coincident inside edge /is;
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "coincident\\s+inside\\s+edge"); 
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			($word, $remain) = ($remain =~ /^\s*(\S+)(.*)$/is);
			$option .= " $word";
		}
		$outLine = &genOutLine("edge_boolean"," -coincident_only -inside $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /(coincident|coin)\s+(out|outside)\s+edge\s+/is) {
		$inLine =~ s/\b(coincident|coin)\s+(outside|out)\s+edge\s+/coincident outside edge /is;
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "coincident\\s+outside\\s+edge"); 
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			($word, $remain) = ($remain =~ /^\s*(\S+)(.*)$/is);
			$option .= " $word";
		}
		$outLine = &genOutLine("edge_boolean"," -coincident_only -outside $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\b(coincident|coin)\s+edge\s+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "(coincident|coin)\\s+edge"); 
		my $option = "";
		my $word = "";
		while ($remain =~ /\S+/) {
			($word, $remain) = ($remain =~ /^\s*(\S+)(.*)$/is);
			$option .= " $word";
		}
		$outLine = &genOutLine("edge_boolean"," -coincident_only $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /touch\s+(in|inside)\s+edge\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) =
			&parseCmdWithNotLayer($inLine, "touch\\s+(in|inside)\\s+edge");
		my $option = "";
		my $value = ""; 
		while ($remain !~ /^\s*$/ ) {
			$remain =~ s/^\s+//;
			if ($remain =~ /^endpoint\s+(\S+)/is) {
				($value, $remain) = ($remain =~ /^endpoint\s+(\S+)\s+(.*)$/is);
				$option .= " -endpoint $value";
			} else {
				($value, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $value";
			}
		}
		$outLine = &genOutLine("edge_select"," -coincident_only -inside $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\btouch\s+(out|outside)\s+edge\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) =
			&parseCmdWithNotLayer($inLine, "touch\\s+(out|outside)\\s+edge");
		my $option = "";
		my $value = ""; 
		while ($remain !~ /^\s*$/ ) {
			$remain =~ s/^\s+//;
			if ($remain =~ /^endpoint\s+(\S+)/is) {
				($value, $remain) = ($remain =~ /^endpoint\s+(\S+)\s+(.*)$/is);
				$option .= " -endpoint $value";
			} else {
				($value, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $value";
			}
		}
		$outLine = &genOutLine("edge_select"," -coincident_only -outside $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\b(in|inside)\s+edge\s+/is) {
		my ($outLay, $notFlag, $layer, $remain) = &parseCmdWithNotLayer($inLine, "(in|inside)\\s+edge");
		my $option = "";
		while ($remain =~ /\S+/is) {
			$remain =~ s/^\s*//is;
			my $word = "";
			($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
			$option .= " $word";
		}
		$outLine = &genOutLine("edge_boolean"," -inside $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\b(out|outside)\s+edge\s+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "(out|outside)\\s+edge");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
			$option .= " $word";
		}
		$outLine = &genOutLine("edge_boolean"," -outside $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bextent\s+drawn\b/is) {
		my ($outLay, $remain) = &parseCmd($inLine, "extent\\s+drawn");
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^original\s+/is) {
				$remain =~ s/^original//is;
				$option .= " -original";
			} elsif ($remain =~ /^ignore\s+/is) {
				$remain =~ s/^ignore//is;
				$option .= " -ignore";
			} else {
				my $word = "";
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("extent_drawn"," $option", $outLay, 2);
		return ($outLine, 0);	
	} elsif ($inLine =~ /\bextent\b/is 
				and $inLine !~ /\bextent\s+(cell|drawn)\b/is ) {
		my ($preKey, $remain) = ($inLine =~ /^\s*(.*)\bextent\b(.*)$/is);
		my $outLay = "";
		my $aftE = "";
		if ($preKey =~ /=/) {
			$outLay = &extractOutLayer($preKey, \$aftE);
		} else {
			$aftE = $preKey;
		}
		if ($aftE =~ /\S/) {
			print "!!CHK_ERR_AFT_EXTENT<<$aftE>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
		}
		my $byLayer = "";
		if ($remain =~ /\S+/) {
			$remain =~ s/\s+//;
			$remain =~ s/\s*$//is;
			$byLayer = " -layer $remain ";
		}
		$outLine = &genOutLine("extent"," $byLayer", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*disconnect/is) {
		$outLine = "disconnect;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*expand\s+cell\s+text\s+\S+/is) {
		my ($key,$remain) = ($inLine =~ /^\s*(expand\s+cell\s+text)\s+(.*)$/is);
		$outLine = "promote_text $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*expand\s+cell\s+\S+/is) {
		my ($key,$remain) = ($inLine =~ /^\s*(expand\s+cell)\s+(.*)$/is);
		$outLine = "promote_cell $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /expand\s+text\s+\S+/is) {
		my ($outLay, $remain) =
				&parseCmd($inLine, "expand\\s+text");
		my $word = "";
		my $option = "";
		my $text = "";
		while ($remain =~ /\S+/is) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^BY\s+(\S+)/is) {
				($word, $remain) = ($remain =~ /^BY\s+(\S+)(.*)$/is);
				$option .= " -by $word";	
			} elsif ($remain =~ /^PRIMARY\s+ONLY/is) {
				($remain) = ($remain =~ /^PRIMARY\s+ONLY(.*)$/is);
				$option .= " -primary_only";
			} elsif ($remain =~ /^CASE\s+sensitive(.*)$/is) {
				($remain) = ($remain =~ /^case\ssensitive(.*)$/is);
				$option .= " -case_sensitive";
			} elsif ($remain =~ /^\"/) {
				($word, $remain) = ($remain =~ /^(\"[^\"]+\")(.*)$/is);
				$text .= " -textname $word";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				if ($text eq "") {
					$text .= " -textname $word";
				} else {
					$option .= " -textlayer $word";
				}
			}
		}
		$outLine = &genOutLine("expand_text_origin"," $text $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bexpand\s+edge\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
				&parseCmdWithLayer($inLine, "expand\\s+edge");
		my $option = "";
		my $value = ""; 
		while ($remain !~ /^\s*$/ ) {
			$remain =~ s/^\s+//;
			if ($remain =~ /^BY\s+\S+/is) {
				($value, $remain) = ($remain =~ /^by\s+(\S+)(.*)$/is);
				$option .= " -by $value";
			} elsif ($remain =~ /^extend\s+by\s+\S+/is) {
				($value, $remain) = ($remain =~ /^extend\s+by\s+(\S+)(.*)$/is);
				$option .= " -extend_by $value";
			} elsif ($remain =~ /^inside\s+by\s+\S+/is) {
				($value, $remain) = ($remain =~ /^inside\s+by\s+(\S+)(.*)$/is);
				$option .= " -inside_by $value";
			} elsif ($remain =~ /^in\s+by\s+\S+/is) {
				($value, $remain) = ($remain =~ /^in\s+by\s+(\S+)(.*)$/is);
				$option .= " -inside_by $value";
			} elsif ($remain =~ /^outside\s+by\s+\S+/is) {
				($value, $remain) = ($remain =~ /^outside\s+by\s+(\S+)(.*)$/is);
				$option .= " -outside_by $value";
			} elsif ($remain =~ /^out\s+by\s+\S+/is) {
				($value, $remain) = ($remain =~ /^out\s+by\s+(\S+)(.*)$/is);
				$option .= " -outside_by $value";
			} elsif ($remain =~ /^corner\s+fill/is) {
				$remain =~ s/^corner\s+fill//is;
				$option .= " -corner_fill";
			} else {
				($value, $remain) = ($remain =~ /^(\S+)(.*)$/);	
				$option .= " $value";
			}
		}
		$outLine = &genOutLine("edge_expand"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\binside\s+cell\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "inside\\s+cell");
		
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^primary\s+only/is) {
				($remain) = ($remain =~ /^primary\s+only(.*)/is);
				$option .= " -primary_only";
			} elsif ($remain =~ /^with\s+layer\s+(\S+)/is) {
				($word, $remain) = ($remain =~ /^with\s+layer\s+(\S+)(.*)/is);
				$option .= " -with_layer $layer";
			} elsif ($remain =~ /^with\s+match\s+golden/is) {
				($remain) = ($remain =~ /^with\s+match\s+golden(.*)/is);
				$option .= " -with_match_golden";
			} elsif ($remain =~ /^with\s+match\s+rule\s+\S+/is) {
				($word, $remain) = ($remain =~ /^with\s+match\s+rule\s+(\S+)(.*)/is);
				$option .= " -with_match_rule $word";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("inside_cell"," $notFlag $layer $option", $outLay, 2);
		return ($outLine, 0);
	} elsif ($inLine =~ /touch\s+edge\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "touch\\s+edge");
		my $option = "";
		my $value = ""; 
		while ($remain !~ /^\s*$/ ) {
			$remain =~ s/^\s+//;
			if ($remain =~ /^endpoint\s+(\S+)/is) {
				($value, $remain) = ($remain =~ /^endpoint\s+(\S+)\s*(.*)$/is);
				$option .= " -endpoint $value";
			} else {
				($value, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $value";
			}
		}
		$outLine = &genOutLine("edge_select"," -coincident_only $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*connect/is) {
		my ($remain) = ($inLine =~ /^\s*connect\s+(.*)$/is);
		$outLine = "connect $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*sconnect/is) {
		my ($remain) = ($inLine =~ /^\s*sconnect\s+(.*)$/is);
		$outLine = "sconnect $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*virtual\s+connect\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*virtual\s+connect\s+(.*)$/is);
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//is;
			if ($remain =~ /^colon\s+\S+/is) {
				($word, $remain) = ($remain =~ /^colon\s+(\S+)(.*)$/is);
				$word = lc($word);
				$option .= " -colon $word";
			} elsif ($remain =~ /^name\s+\S+/is) {
				($remain) = ($remain =~ /^name\s+(.*)$/is);
				$option .= " -name $remain";
				$remain = "";
			} elsif ($remain =~ /^depth\s+\S+/is) {
				($word, $remain) = ($remain =~ /^depth\s+(\S+)(.*)$/is);
				$word = lc($word);
				$option .= " -depth $word";
			} elsif ($remain =~ /^incremental\s+\S+/is) {
				($word, $remain) = ($remain =~ /^incremental\s+(\S+)(.*)$/is);
				$word = lc($word);
				$option .= " -incremental $word";
			} elsif ($remain =~ /^report\s+maximum\s+\S+/is) {
				($word, $remain) = ($remain =~ /^report\s+maximum\s+(\S+)(.*)$/is);
				$option .= " -report_maximum $word";
			} elsif ($remain =~ /^report\s+\S+/is) {
				($word, $remain) = ($remain =~ /^report\s+(\S+)(.*)$/is);
				$word = lc($word);
				$option .= " -report $word";
			} elsif ($remain =~ /^semicolon\s+as\s+colon\s+\S+/is) {
				($word, $remain) = ($remain =~ /^semicolon\s+as\s+colon\s+(\S+)(.*)$/is);
				$word = lc($word);
				$option .= " -semicolon_as_colon $word";
			} else {
				$option .= " $remain";
				$remain = "";
			}	
		}
		$outLine = "virtual_connect $option;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\bor\s+edge\s+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "or\\s+edge");
		my $option = $remain;
		$option =~ s/\s*$//;
		$outLine = &genOutLine("or_edge"," $layer $option", $outLay, 1);
		return ($outLine, 0);	
	} elsif ($inLine =~ /\bmerge\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "merge");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^BY\s+\S+/is) {
				($word, $remain) = ($remain =~ /^BY\s+(\S+)(.*)$/is);
				$option .= " -by $word";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}	
		$outLine = &genOutLine("merge"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*cmacro\s+\S+/is) {
		my ($remain) = ($inLine =~ /^\s*cmacro\s+(.*)$/is);
		$outLine = "cmacro $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\bsize\s+\S+/is) {
		my ($outLay, $inLay, $remain) = 
			&parseCmdWithLayer($inLine, "size");

		my $option = "";
		my $value = "";
		my $layer = "";
		my $step = "";
		while ($remain !~ /^\s*$/ ) {
			$remain =~ s/^\s+//;
			if ($remain =~ /^BY\s+\S+/is) {
				($value, $remain) = ($remain =~ /^by\s+(\S+)(.*)$/is);
				$option .= " -by $value";
			} elsif ($remain =~ /^INSIDE\s+OF\s+\S+/is) {
				($layer, $remain) = ($remain =~ /^INSIDE\s+OF\s+(\S+)(.*)$/is);
				$option .= " -inside_of $layer";
			} elsif ($remain =~ /^outside\s+OF\s+\S+/is) {
				($layer, $remain) = ($remain =~ /^outside\sOF\s+(\S+)(.*)$/is);
				$option .= " -outside_of $layer";
			} elsif ($remain =~ /^STEP\s+\S+/is) {
				($step, $remain) = ($remain =~ /^STEP\s+(\S+)(.*)$/is);
				$option .= " -step $step";
			} elsif ($remain =~ /^BEVEL\s+\S+/is) {
				($step, $remain) = ($remain =~ /^BEVEL\s+(\S+)(.*)$/is);
				$option .= " -bevel $step";
			} elsif ($remain =~ /^TRUNCATE\s+\S+/is) {
				my $trunc = "";
				($trunc, $remain) = ($remain =~ /^TRUNCATE\s+(\S+)(.*)$/is);
				$option .= " -truncate $trunc";
			} elsif ($remain =~ /^OVERUNDER/is) {
				$option .= " -overunder";
				$remain =~ s/^OVERUNDER\s*//is;
			} elsif ($remain =~ /^UNDEROVER/is) {
				$option .= " -underover";
				$remain =~ s/^UNDEROVER\s*//is;
			} elsif ($remain =~ /^overlap\s+only/is) {
				$option .= " -overlaponly";
				$remain =~ s/^overlap\s+only//is;
			} else {
				my $word = "";
				if ($remain =~ /\s*\S+\s+.*$/) {
					($word, $remain) = ($remain =~ /\s*(\S+)\s+(.*)$/);
				} else {
					$word = $remain;
					$word =~ s/^\s+//;
					$word =~ s/\s+$//;
					$remain = "";
				}
				$option .= "$word";
			}
		}
		$outLine = &genOutLine("size"," $inLay $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bcopy\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "copy");
		$outLine = &genOutLine("copy"," $layer", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bholes\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "holes");

		my $option = "";
		my $haveOp = 0;
		my $word = "";
		while ($remain !~ /^\s*$/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^inner/is) {
				$option .= " -inner";
				$remain =~ s/^inner//is;
			} elsif ($remain =~ /^empty/is) {
				$option .= " -empty";
				$remain =~ s/^empty//is;
			} elsif ($remain =~ /^singular\s+also/is) {
				$option .= " -singular_also";
				$remain =~ s/^singular\s+also//is;
			} elsif ($remain =~ /^[><=!]/) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
				my ($isOp, $resOp) = &transRelOp($word);
				$option .= $resOp;
				$haveOp = 1;	
			} elsif ($haveOp == 1) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
				$option .= " $word";
				$haveOp = 0;
			} else {
				$option .= " !!$remain!!";
				$remain = "";
			}
		}
		$outLine = &genOutLine("holes"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /path\s+length\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "path\\s+length");
		my $option = "";
		my $value = ""; 
		while ($remain !~ /^\s*$/ ) {
			my $word = "";
			#print ">>$remain\n";
			if ($remain =~ /(\S+)\s+(\S+)\s*$/) {
				($word, $remain) = ($remain =~ /(\S+)\s+(.*)$/);
			} else {
				$word = $remain;
				$word =~ s/\s+//g;
				$remain = "";
			}
			my ($isOp, $resOp) = &transRelOp($word);
			$option .= " $resOp";
		}
		$outLine = &genOutLine("path_length"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif($inLine =~ /^\s*unit\s+\S+/is) {
		my ($type, $value) = ($inLine =~ /^\s*unit\s+(\S+)\s+(\S+)/is);
		my $outType = "";
		if ($type =~ /CAPACITANCE/is) {
			$outType = " -cap";
		} elsif ($type =~ /LENGTH/is) {
			$outType = " -length";
		} elsif ($type =~ /RESISTANCE/is) {
			$outType = " -res";
		} elsif ($type =~ /TIME/is) {
			$outType = " -time";
		} else {
			$outType = " -".lc($type);
		}		
		$outLine = "unit $outType $value;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\blength\s+\S+/is) {
		if ($inLine =~ /\bwith\s+length\b/is) {
			return ($inLine, 1);
		}
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "length");
		my $option = "";
		my $value = ""; 
		if ($layer =~ /^[<>=!]+$/) {
			my ($word, $realLayer, $realRemain) = ($remain=~ /^\s*(\S+)\s+(\S+)(.*)$/is);
			$remain = "$layer $word $realRemain";
			$layer = $realLayer;
		} elsif ($layer =~ /^[<>=!]+\S+/) {
			my ($realLayer, $realRemain) = ($remain =~ /^\s*(\S+)(.*)$/is);
			$remain = "$layer $realRemain";
			$layer = $realLayer;
		}
		while ($remain =~ /\S+/) {
			my $word = "";
			$remain =~ s/^\s*//;
			#print ">>$remain\n";
			($word, $remain) = ($remain =~ /(\S+)(.*)$/is);
			if ($word =~ /^[<>=!]/) {
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					print "!!CHK_PARSE<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} else {
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("edge_length"," $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bangle\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
				&parseCmdWithNotLayer($inLine, "angle");
		my $option = "";
		my $value = ""; 
		while ($remain =~ /\S+/ ) {
			my $word = "";
			$remain =~ s/^\s*//;
			#print ">>$remain\n";
			if ($remain =~ /^[<>=!]/) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);	
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					print "!!CHK_PARSE<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);	
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("angle"," $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /^\s*litho\s+file\s+\S+/is) {
		my ($key, $file, $content) = ($inLine =~ /^\s*(litho\s+file)\s+(\S+)\s+(.*)$/is);
		$outLine = "litho_file $file $content;\n";
		return ($outLine, 0);	
	} elsif ($inLine =~ /\bwith\s+text\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "with\\s+text");
		my $textName = "";
		#if ($label !~ /^".*"$/is and $label !~ /\?/is) {
		#	$remain = "$label $remain";
		#} else {
			#$textName = " -textname $label";
		#}	
		#my $inLay = "";
		
		my $option = "";
		my $word = "";
		my $textLayer = "";
		while ($remain !~ /^\s*$/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^PRIMARY\s+ONLY/is) {
				$option .= " -primary_only";
				$remain =~ s/^PRIMARY\s+ONLY//;
			} elsif ($remain =~ /CASE\s+SENSITIVE/is) {
				$option .= " -case_sensitive";
				$remain =~ s/^CASE\s+SENSITIVE//;
			} else {
				($word, $remain) = ($remain=~ /(\S+)(.*)$/is);
				if ($textName eq "") {
					$textName = " -textname $word"; 
				} else {
					if ($word =~ /^"/) {
						$textName .= " $word";
					} else {
						$textLayer = " -textlayer $word"; 
					}
				}
			}
		}
		$outLine = &genOutLine("select"," -label $notFlag $textName $textLayer $option $layer", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\benclose\s+rectangle\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "enclose\\s+rectangle");
		my $option = "";
		my $word = "";
		my $width = "";
		my $length = "";
		while ($remain !~ /^\s*$/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^orthogonal\s+only/is) {
				$remain =~ s/^orthogonal\s+only//is;
				$option .= " -orthogonal_only";
			} elsif ($remain =~ /^fixed\b/is) {
				$remain =~ s/^fixed\s*//is;
				$option .= " -fixed";
			} else {
				if ($width eq "") {
					my ($value, $good) = &getCompleteValue(\$remain);
					if ($good == 1) {
						$width = $value;
					} else {
						print "!!CHK_PARSE<<$value>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
					}
				} elsif ($length eq "") {
					my ($value, $good) = &getCompleteValue(\$remain);
					if ($good == 1) {
						$length = $value;
					} else {
						print "!!CHK_PARSE<<$value>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
					}
				} else {
					($word, $remain) = ($remain =~ /^\s*(\S+)(.*)/is);
					print "!!CHK_PARSE<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
					$option .= "!!OPT_ERR<<$word>>";
				}
				#$option .= " -length $length -width $width";
			}
		}
		$outLine = &genOutLine("select"," -enclose_rect $notFlag -length $length -width $width $option $layer", $outLay, 1);
		return ($outLine, 0);
	
	} elsif ($inLine =~ /\brectangle\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "rectangle");
		if ($layer =~ /[<>=]$/) {
			my $word;
			($layer,$word) = ($layer =~ /^\s*([^<>=]+)([<>=]+)/);
			$remain = $word." ".$remain;
		}
		my $option = "";
		my $value = ""; 
		while ($remain =~ /\S+/ ) {
			my $word = "";
			$remain =~ s/^\s*//;
			#print ">>$remain\n";
			if ($remain =~ /^BY\s+/is) {
				$option .= " -by";
				$remain =~ s/^BY\s+//is;
			} elsif ($remain =~ /^orthogonal\s+only/is) {
				$option .= " -orthogonal_only";
				$remain =~ s/^orthogonal\s+only//is;
			} elsif ($remain =~ /^measure\s+extents/is) {
				$option .= " -mearure_extents";
				$remain =~ s/^measure\s+extents//is;
			} elsif ($remain =~ /^aspect\s+\S+/is) {
				$remain =~ s/^aspect\s+//is;
				$option .= " -aspect";
			} elsif ($remain =~ /^[<>=!]/) {
				($word, $remain) = ($remain =~ /(\S+)(.*)$/);
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) { 
					$option .= " $resOp";
				} else {
					print "!!CHK_PARSE<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} else {
				($word, $remain) = ($remain =~ /(\S+)(.*)$/);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("rect_chk"," $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\barea\s+\S+/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "area");
			
		my $option = "";
		my $value = ""; 
		while ($remain =~ /\S+/ ) {
			my $word = "";
			$remain =~ s/^\s*//is;
			if ($remain =~ /^[<>=!]/) {
				($value, $remain) = &transRptRelOp($remain, $inLine);
				$option .= " $value";
			} else {
				($word, $remain) = ($remain =~ /(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("area"," $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bwith\s+edge\b/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "with\\s+edge");
		if ($layer eq "") {
			return ("", 2);
		}
		my $option = "";
		my $value = ""; 
		while ($remain =~ /\S+/ ) {
			$remain =~ s/^\s*//;
			my $word = "";
			#print ">>$remain\n";
			($word, $remain) = ($remain =~ /(\S+)(.*)$/is);
			if ($word =~ /^[<>=!]/) {
				my ($isOp, $resOp) = &transRelOp($word);
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					print "!!CHK_PARSE<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} else {
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("select"," -with_edge $notFlag $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bstamp\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
				&parseCmdWithLayer($inLine, "stamp");
		my $option = "";
		my $word = "";
		my $value = ""; 
		my $layer2 = "";
		while ($remain =~ /\S+/) {
			my $word = "";
			$remain =~ s/^\s*//;
			#print ">>$remain\n";
			if ($remain =~ /^BY\s+\S+/) {
				($layer2, $remain) = ($remain =~ /^BY\s+(\S+)(.*)$/);
			} elsif ($remain =~ /^ABUT\s+ALSO\b/is) {
				$remain =~ s/^abut\s+also\b//is;
				$option .= " -abut_also";
			} elsif ($remain =~ /^[<>=!]/) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
				my ($isOp, $resOp) = &transRelOp($word);
				$option .= " $resOp";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("stamp"," $layer $layer2 $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bsnap\b/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "snap");
		my $option = "";
		my $word = "";
		while ($remain !~ /^\s*$/) {
			$remain =~ s/^\s+//;
			if ($remain =~ /(\S+)\s+(\S+)$/) {
				($word, $remain) = ($remain =~ /(\S+)\s+(.*)$/);
			} else {
				$word = $remain;
				$word =~ s/\s+//g;
				$remain = "";
			}
			$option .= " $word";
		}
		$outLine = &genOutLine("snap"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bnet\s+(\S+)/is) {
		my ($outLay, $notFlag, $layer, $remain) = 
			&parseCmdWithNotLayer($inLine, "net");
		$remain =~ s/^\s*//;
		if ($layer eq "") {
			($layer, $remain) = ($remain =~ /^(\S+)(.*)/);
		}
		$remain =~ s/\s*$//is;
		$outLine = &genOutLine("select_net"," $notFlag $layer $remain", $outLay, 2);
		return ($outLine, 0);
	} elsif ($inLine =~ /(^|\s+)TDDRC\s+\S+/is) {
		my ($outLay, $layer, $remain) = &parseCmdWithLayer($inLine, "tddrc");
		my $word = "";
		my $option = "";
		if ($layer =~ /^\[.*\]/) {
			my ($r) = ($layer =~ /^\[(.*)\]/);
			$layer = $r;
			$option .= " -output positive1";
		} elsif ($layer =~ /^\(.*\)/) {
			my ($r) = ($layer =~ /^\((.*)\)/);
			$layer = $r;
			$option .= " -output negative1";
		}
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^region\b/is) {
				$remain =~ s/^region\b//is;
				$option .= " -output region ";
			} elsif ($remain =~ /^BY\s+polygon\b/is) {
				$remain =~ s/^by\s+polygon\b//is;
				$option .= " -by_polygon";
			} elsif ($remain =~ /^measure\s+all\b/is) {
				$remain =~ s/^measure\s+all\b//is;
				$option .= " -measure_all";
			} elsif ($remain =~ /^space\b/is) {
				$remain =~ s/^space\b//is;
				$option .= " -space";
			} elsif ($remain =~ /^width1\b/is) {
				$remain =~ s/^width1\b//is;
				$option .= " -width1";
			} elsif ($remain =~ /^width2\b/is) {
				$remain =~ s/^width2\b//is;
				$option .= " -width2";
			} elsif ($remain =~ /^width\b/is) {
				$remain =~ s/^width\b//is;
				$option .= " -width";
			} elsif ($remain =~ /^exclude\s+shielded\s+\S+/is) {
				($word, $remain) = ($remain =~ /^exclude\s+shielded\s+(\S+)(.*)$/is);
				$option .= " -exclude_shielded $word";
			} elsif ($remain =~ /^opposite\s+extended\s+\S+/is) {
				($word, $remain) = ($remain =~ /^opposite\s+extended\s+(\S+)(.*)$/is);
				$option .= " -opposite_extented $word";
			} elsif ($remain =~ /^square\b/is) {
				$remain =~ s/^square\b//is;
				$option .= " -square";
			} elsif ($remain =~ /^opposite\b/is) {
				$remain =~ s/^opposite\b//is;
				$option .= " -opposite";
			} elsif ($remain =~ /^projecting\b/is) {
				$remain =~ s/^projecting\b//is;
				$option .= " -projecting";
			} elsif ($remain =~ /^[<>=!]/is) {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				my ($isOp, $resOp) = (0, "");
				if ($option =~ /projecting$/) {
					($isOp, $resOp) = &transAbutRelOp($word);
				} else {
					($isOp, $resOp) = &transRelOp($word);
				}
				if ($isOp > 0) {
					$option .= " $resOp";
				} else {
					print "!!CHK_PARSE<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
				}
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}	
		}
		$outLine = &genOutLine("wdsc"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bflatten\s+\S+/is) {
		my ($outLay, $layer, $remain) =	
			&parseCmdWithLayer($inLine, "flatten");
		$outLine = &genOutLine("flatten"," $layer", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /(\b|\s+)push\s+\S+/is) {	
		my ($outLay, $layer, $remain) = &parseCmdWithLayer($inLine, "push");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^locate\s+\S+/is) {
				($word, $remain) = ($remain =~ /^locate\s+(\S+)(.*)$/is);
				$option .= " -locate";
			} elsif ($remain =~ /^light/is) {
				$remain =~ s/light//is;
			} elsif ($remain =~ /^medium/is) {
				$remain =~ s/medium//is;
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/);
			}
		}
		$outLine = &genOutLine("push"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bret\s+nmdpc\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "ret\\s+nmdpc");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^file\s+/is) {
				($word, $remain) = ($remain =~ /^file\s+(\S+)(.*)$/is);
				$option .= " -file $word";
			} elsif ($remain =~ /^map\s+/is) {
				($word, $remain) = ($remain =~ /^map\s+(\S+)(.*)$/is);
				$option .= " -map $word";
			} else { 
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("ret_nmdpc"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bret\s+dp\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "ret\\s+dp");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^file\s+/is) {
				($word, $remain) = ($remain =~ /^file\s+(\S+)(.*)$/is);
				$option .= " -file $word";
			} elsif ($remain =~ /^map\s+/is) {
				($word, $remain) = ($remain =~ /^map\s+(\S+)(.*)$/is);
				$option .= " -map $word";
			} else { 
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("ret_dp"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\bret\s+tp\s+\S+/is) {
		my ($outLay, $layer, $remain) = 
			&parseCmdWithLayer($inLine, "ret\\s+tp");
		my $word = "";
		my $option = "";
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^file\s+/is) {
				($word, $remain) = ($remain =~ /^file\s+(\S+)(.*)$/is);
				$option .= " -file $word";
			} elsif ($remain =~ /^map\s+/is) {
				($word, $remain) = ($remain =~ /^map\s+(\S+)(.*)$/is);
				$option .= " -map $word";
			} else { 
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}
		}
		$outLine = &genOutLine("ret_tp"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif ($inLine =~ /\blitho\s+file\s+/is) {
		my ($name, $remain) = ($inLine =~ /^\s*litho\s+file\s+(\S+)(.*)$/is);
		$outLine = "litho_file $name $remain;\n";
		return ($outLine, 0);
	} elsif ($inLine =~ /\bshift\s+\S+/is) {
		my ($outLay, $layer, $remain) =
			&parseCmdWithLayer($inLine, "shift");
		my ($word, $option) = ("", "");
		while ($remain =~ /\S+/) {
			$remain =~ s/^\s*//;
			if ($remain =~ /^by\s+/is) {
				$remain =~ s/^by\s+//is;
				$option .= " -by ";
			} else {
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				$option .= " $word";
			}	
		}
		$outLine = &genOutLine("shift"," $layer $option", $outLay, 1);
		return ($outLine, 0);
	} elsif (&parseDrcCmd($inLine, \$outLine) == 1) {
		return ($outLine, 0);
	} elsif (&parseTopoCmd($inLine, \$outLine) == 1) {
		return ($outLine, 0);
	} elsif (&parseLogicCmd($inLine, \$outLine) == 1) {
		return ($outLine, 0);
	} else { # unrecoginized command
		$incomplete = 1;
		#$refFlag->{incomplete} = 1;
		if (exists $Options{check}) {
			print "!!!!CHK_PUZZLE BEGIN!!!<$outLine!!!!CHK_PUZZLE END!!!";
		}
		return ("!!!CHK_PUZZLE BEGIN!!!<$outLine!!!CHK_PUZZLE END!!!", 1);
	}
}

sub parseTopoCmd {
	my ($inLine, $refOut) = @_;
	my @cmdKeys = ("interact", "outside", "inside", "touch", "cut", "enclose", "in", "out");
	foreach my $keys (@cmdKeys) {
		if ($inLine =~ /\b$keys\s+\S+/is) {
			my ($pre, $remain) = ($inLine =~ /^\s*(.*)\b$keys\b\s+(.*)$/is);
			if ($remain =~ /^also/is) {
				# for outside also
				next;
			}
			if ($remain =~ /^by/is) {
				next;
			}
			if ($remain =~ /^of\b/is) {
				next;
			}
			my $outLay = "";
			my $neg = "";
			my $aftE = "";
			if ($pre =~ /=/) {
				$outLay = &extractOutLayer($pre, \$aftE);
			} else {
				$aftE = $pre;
			}
			if ($aftE =~ /not\s*$/is) {
				$neg = "-not";
				$aftE =~ s/not\s*$//is;
			}
			my $first = "";
			my $keyword = "$keys";
			if ($keys =~ /^in\b/is) {
				$keyword = "inside";
			} elsif ($keys =~ /out/is) {
				$keyword = "outside";
			}
			if ($aftE =~ /\S+/) {
				$first = $aftE;
			}
			my $option = "";
			while ($remain =~ /\S+/) {
				$remain =~ s/^\s*//;
				my $word = "";
				($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
				if ($word =~ /^\w/ and $word =~ /[<>=!]/) {
					my ($w1, $o1) = ($word =~ /^([^<>=!]+)([<>=!].*)/is);
					$word = $w1;
					$remain = $o1.$remain;
				}
				if ($word =~ /^[<>=!]/) {
					my ($isOp, $resOp) = &transRelOp($word);
					if ($isOp > 0) {
						$option .= " $resOp";
					} else {
						print "!!CHK_WORD>>$word<<!!CHK_WORD in !!INLINE_BEGIN>>$inLine<<INLINE_END!!\n";
					}
				} else {
					if ($word =~ /^BY$/is and $remain=~ /^\s*NET/is) {
						 $option .= " -by_net";
						$remain =~ s/^\s*NET//is;
					} elsif ($word =~ /^SINGULAR$/is) {
						if ($remain =~ /^\s*ALSO/is) {
							$option .= " -single_point also";
							$remain =~ s/^\s*ALSO//is;
						} elsif ($remain =~ /^\s*only/is) {
							$option .= " -single_point only";
							$remain =~ s/^\s*ONLY//is;
						} else {
							print " !!! SINGLUAR ERROR!!!";
						}
					} elsif ($word =~ /^even\b/is) {
							$option .= " -even";
					} elsif ($word =~ /^odd\b/is) {
							$option .= " -odd";
					} else {
							$option .= " $word";
					}
				}
			}
			$$refOut = &genOutLine("select"," -$keyword $neg $first $option", $outLay, 1);
			return 1;
		}
	}
	return 0;
}

sub parseLogicCmd {
	my ($inLine, $refOut) = @_;
	my @cmdKeys = ("and", "or", "not", "xor") ;
	foreach my $keys (@cmdKeys) {
		if ($inLine =~ /\b$keys\s+\S+/is) {
			my ($preKey, $aftKey) = ($inLine =~ /^\s*(.*)\b$keys\b(.*)/is);
			my $outLay = "";
			my $aftE = "";
			my $keyword = "$keys";
			if ($preKey =~ /=/) {
				$outLay = &extractOutLayer($preKey, \$aftE);
			} else {
				$aftE = $preKey;
			}
			my $layer = "";
			if ($aftE =~ /\S+/) {
				($layer) = ($aftE =~ /(\S+)\s*$/);
			}
			my $option = "";
			my $remain = $aftKey;
			while ($remain =~ /\S+/) {
				$remain =~ s/^\s*//;
				if ($remain =~ /^not\s+connected\b/is) {
					$remain =~ s/^not\s+connected\b//is;
					$option .= " -not_connected";
				} elsif ($remain =~ /^connected\b/is) {
					$remain =~ s/^connected\b//is;
					$option .= " -connected";
				} else {
					my $word;
					($word, $remain) = ($remain =~ /^(\S+)(.*)/is);
					$option .= " $word";
				}
			}	
			#$aftKey =~ s/\s+$//is;
			$$refOut = &genOutLine("$keyword"," $layer $option", $outLay, 1);
			return 1;
		} elsif ($inLine =~ /\b$keys\s+\S+/is) {
			my ($pre, $aft, $remain) = ($inLine =~ /^\s*(.*)$keys\s+(\S+)(.*)$/is);
			if ($remain !~ /\S/) {
				return 0;
			}
			my $outLay = "";
			my $first = $pre;
			my $keyword = "$keys";
			if ($pre =~ /\S+\s*=\s*\S+/) {
				my ($out, $in1) = ($pre =~ /(\S+)\s*=\s*(\S+)/);
				$outLay = $out;
				$first = $in1;
			}
			$$refOut = &genOutLine("$keyword"," $first $aft $remain", $outLay, 1);
			return 1;
		} else {
		}
	}
	return 0;
}

sub parseDrcCmd {
	my ($inLine, $refOut) = @_;
	my %cmdKeys = (
			"(enc|enclosure)" =>  	"enc",
			"(ext|external)" =>	"exte",
			"(int|internal)"  =>  "inte", 
#			"enc" =>  	"enc",
#			"ext" => 	"exte", 
#			"int" => 	"inte", 
#			"enclosure" =>	"enc", 
#			"internal"  =>  "inte", 
#			"external" =>	"exte",
			);
	foreach my $keys (keys %cmdKeys) {
		if ($inLine =~ /\b$keys\s+\S+/is) {
			#my ($pre, $remain) = ($inLine =~ /^\s*(.*)$keys\s+(.*)$/is);
			my ($pre, $matchKey, $remain) = ($inLine =~ /^\s*(.*)\b$keys\s+(.*)$/is);
			my $outLay = "";
			my $first = $pre;
			my $keyword = $cmdKeys{$keys};
			if ($pre =~ /\S+\s*=\s*\S+/) {
				my ($out, $in1) = ($pre =~ /(\S+)\s*=\s*(\S+)/);
				$outLay = $out;
				$first = $in1;
			} elsif ($pre =~ /\S+\s*=\s*$/) {
				($outLay) = ($pre =~ /(\S+)\s*=\s*$/);
				$first = "";
			}
			my $option = "";
			my $onCons = 0;
			my $onAbut = 0;
			my $wIndexRemain = 1;
			my $secondLayer = "";
			while ($remain =~ /\S+/is) {
				$remain =~ s/^\s*//is;	
				my $word = "";
				#print ">>$remain\n";
				if ($remain =~ /^[<>=!]/) {
					($word, $remain) = ($remain =~ /(\S+)(.*)$/is);
					my $isOp = 0;
					my $resOp = "";
					if ($onAbut == 0) {
						($isOp, $resOp) = &transRelOp($word);
					} else {
						($isOp, $resOp) = &transAbutRelOp($word);
					}
					if ($isOp > 0) {
						$option .= " $resOp";
					} else {
						print "!!CHK_DRC_WORD>>$word<<CHK_DRC_WORD!! in !!INLINE_BEGIN>>$inLine<<INLINE_END!!\n";
					}
				} elsif ($remain =~ /^abut\b/is) {
					$remain =~ s/^abut\b//is;
					$option .= " -abut";
					$onAbut = 1;
				} elsif ($remain =~ /^region\s+extents\b/is) {
					$remain =~ s/^region\s+extents\b//is;
					$option .= " -output region_extents";
					$onAbut = 0;
				} elsif ($remain =~ /^region\s+centerline\s+\S+/is) {
					($word, $remain) = ($remain =~ /^region\s+centerline\s+(\S+)(.*)$/is);
					$option .= " -output region_centerline $word";
					$onAbut = 0;
				} elsif ($remain =~ /^region\s+centerline\b/is) {
					$remain =~ s/^region\s+centerline//is;
					$option .= " -output region_centerline";
					$onAbut = 0;
				} elsif ($remain =~ /^region\b/is) {
					$remain =~ s/^region\b//is;
					$option .= " -output region";
					$onAbut = 0;
				} elsif ($remain =~ /^overlap\b/is) {
					$remain =~ s/^overlap\b//is;
					$option .= " -overlap";
					$onAbut = 0;
				} elsif ($remain =~ /^singular\b/is) {
					$remain =~ s/^singular//is;
					$option .= " -dot_touch";
					$onAbut = 0;
				} elsif ($remain =~ /^NOTCH\b/is) {
					$remain =~ s/^notch\b//is;
					$option .= " -notch";
					$onAbut = 0;
				} elsif ($remain =~ /^space\b/is) {
					$remain =~ s/^space\b//is;
					$option .= " -space";
					$onAbut = 0;
				} elsif ($remain =~ /^corner\b/is) {
					$remain =~ s/^corner\b//is;
					$option .= " -corner";
					$onAbut = 0;
				} elsif ($remain =~ /^connected\b/is) {
					$remain =~ s/^connected\b//is;
					$option .= " -connected";
					$onAbut = 0;
				} elsif ($remain =~ /^inside\s+also\b/is) {
					$remain =~ s/^inside\s+also\b//is;
					$option .= " -inside_also";
					$onAbut = 0;
				} elsif ($remain =~ /^outside\s+also/is) {
					$remain =~ s/^outside\s+also//is;
					$option .= " -outside_also";
					$onAbut = 0;
				} elsif ($remain =~ /^intersecting\s+only/is) {
					$remain =~ s/^intersecting\s+only//is;
					$option .= " -intersecting only";
					$onAbut = 0;
				} elsif ($remain =~ /^(PERP|perpendicular)\s+only/is) {
					$remain =~ s/^(PERP|perpendicular)\s+only//is;
					$option .= " -perp only";
					$onAbut = 0;
				} elsif ($remain =~ /^(PERP|perpendicular)\s+also/is) {
					$remain =~ s/^(PERP|perpendicular)\s+also//is;
					$option .= " -perp also";
					$onAbut = 0;
				} elsif ($remain =~ /^NOT\s+(perp|perpendicular)/is) {
					$remain =~ s/^NOT\s+(perp|perpendicular)\s+//is;
					$option .= " -perp not";
					$onAbut = 0;
				} elsif ($remain =~ /^NOT\s+connected/is) {
					$remain =~ s/^NOT\s+connected//is;
					$option .= " -not_connected";
					$onAbut = 0;
				} elsif ($remain =~ /^NOT\s+corner/is) {
					$remain =~ s/^NOT\s+corner//is;
					$option .= " -not_corner";
					$onAbut = 0;
				} elsif ($remain =~ /^NOT\s+(PROJ|projecting)/is) {
					$remain =~ s/^NOT\s+(PROJ|projecting)//is;
					$option .= " -not_proj";
					$onAbut = 0;
				} elsif ($remain =~ /^OPPOSITE\b/is) {
					$remain =~ s/^OPPOSITE\b//is;
					$option .= " -metric opposite";
					$onAbut = 0;
				} elsif ($remain =~ /^exclude\s+shielded\s+\S+/is) {
					($word, $remain) = ($remain =~ /^exclude\s+shielded\s+(\S+)(.*)$/is);
					$option .= " -exclude_shielded $word";
				} elsif ($remain =~ /^exclude\s+shielded\s*$/is) {
					$remain =~ s/^exclude\s+shielded//is;
					$option .= " -exclude_shielded";
				} elsif ($remain =~ /^(PROJ|projecting)\b/is) {
					$remain =~ s/^(PROJ|projecting)\b//is;
					$option .= " -proj";
					$onAbut = 1;
				} elsif ($remain =~ /^(PARA|parallel)\s+also\b/is) {
					$remain =~ s/^(para|parallel)\s+also\b//is;
					$option .= " -parallel also";
					$onAbut = 0;
				} elsif ($remain =~ /^(PARA|parallel)\s+only\b/is) {
					$remain =~ s/^(para|parallel)\s+only\b//is;
					$option .= " -parallel only";
					$onAbut = 0;
				} elsif ($remain =~ /^not\s+(PARA|parallel)\b/is) {
					$remain =~ s/^not\s+(para|parallel)\b//is;
					$option .= " -parallel not";
					$onAbut = 0;
				} elsif ($remain =~ /^exclude\s+false/is) {
					$remain =~ s/^exclude\s+false//is;
					$option .= " -exclude_false_notch";
					$onAbut = 0;
				} elsif ($remain =~ /^measure\s+all/is) {
					$remain =~ s/^measure\s+all//is;
					$option .= " -measure all";
					$onAbut = 0;
				} else {
					my $haveStickyOper = 0;
					if ($first eq "") {
						my ($layer, $isLay) = ("", 0);
						($layer, $remain, $isLay) = &getLayer($remain);
						if ($isLay == 0) {
							print "!!CHK_DRC_LAYER<<$remain>> from !!INLINE_BEGIN>>$inLine<<INLINE_END!!\n";
						}
						$first = $layer;
						$wIndexRemain = 0;
					} else {
						if ($wIndexRemain == 1) {
							my ($layer, $isLay) = ("", 0);
							($layer, $remain, $isLay) = &getLayer($remain);
							if ($isLay == 0) {
								print "!!CHK_DRC_LAYER2<<$remain>> from !!INLINE_BEGIN>>$inLine<<INLINE_END!!\n";
							}
							$secondLayer = $layer;
						} else {
							if ($remain =~ /^\S+?[<>=!]/) {
								($word, $remain) = ($remain =~ /^(\S+?)([<>=!].*)$/is);
								$haveStickyOper = 1;
							} else {
								($word, $remain) = ($remain =~ /^(\S+)(.*)$/is);
							}
							$option .= " $word";
						}
					}
					if ($haveStickyOper == 0) {
						$onAbut = 0;
					}
				}
				#print "--$remain--\n";
				#print ">>$remain<<\n";
				++$wIndexRemain;
			}
			if ($first =~ /^\[.*\]$/) {
				$first =~ s/^\[//;
				$first =~ s/\]$//;
				$option .= " -output positive1";
			} elsif ($first =~ /^\(.*\)$/) {
				$first =~ s/^\(//;
				$first =~ s/\)$//;
				$option .= " -output negative1";
			} elsif ($secondLayer =~ /^\[.*\]$/) {
				$secondLayer =~ s/^\[//;
				$secondLayer =~ s/\]$//;
				$option .= " -output positive2";
			} elsif ($secondLayer =~ /^\(.*\)$/) {
				$secondLayer =~ s/^\(//;
				$secondLayer =~ s/\)$//;
				$option .= " -output negative2";
			}
				
			if ($secondLayer !~ /^\s*$/) {
				$$refOut = &genOutLine("$keyword"," $first $secondLayer"."$option", $outLay, 1);
			} else {
				$$refOut = &genOutLine("$keyword"," $first $option", $outLay, 1);
				#$$refOut = "$keyword $first $first $option $outLay;\n";
			}
			return 1;
		}
	}
	return 0;
}

sub isCommandInParenthesis {
	my ($line) = @_;
	if ($line =~ /\bsize\b/is) {
		return 1;
	} elsif ($line =~ /\band\b/is) {
		return 1;
	} elsif ($line =~ /\bnot\b/is) {
		return 1;
	} elsif ($line =~ /\bxor\b/is) {
		return 1;
	} elsif ($line =~ /\bor\b/is) {
		return 1;
	} elsif ($line =~ /\boutside\b/is) {
		return 1;
	} elsif ($line =~ /\bout\b/is) {
		return 1;
	} elsif ($line =~ /\bcut\b/is) {
		return 1;
	} elsif ($line =~ /\bholes\b/is) {
		return 1;
	} elsif ($line =~ /\bangle\b/is) {
		return 1;
	} elsif ($line =~ /\brotate\b/is) {
		return 1;
	} elsif ($line =~ /\binteract\b/is) {
		return 1;
	} elsif ($line =~ /^\s*inside\b/is) {
		return 1;
	} elsif ($line =~ /\bin\b/is) {
		return 1;
	} elsif ($line =~ /\benclose\b/is) {
		return 1;
	} elsif ($line =~ /\bshrink\b/is) {
		return 1;
	} elsif ($line =~ /\bgrow\b/is) {
		return 1;
	} elsif ($line =~ /\bvertex\b/is) {
		return 1;
	} elsif ($line =~ /\barea\b/is) {
		return 1;
	} elsif ($line =~ /\bext\b/is) {
		return 1;
	} elsif ($line =~ /\benc\b/is) {
		return 1;
	} elsif ($line =~ /\bint\b/is) {
		return 1;
	} elsif ($line =~ /\binternal\b/is) {
		return 1;
	} elsif ($line =~ /\bexternal\b/is) {
		return 1;
	} elsif ($line =~ /\benclosure\b/is) {
		return 1;
	} elsif ($line =~ /\bornet\b/is) {
		return 1;
	} elsif ($line =~ /\blength\b/is) {
		return 1;
	} elsif ($line =~ /\bholes\b/is) {
		return 1;
	} elsif ($line =~ /\bextents\b/is) {
		return 1;
	} elsif ($line =~ /\bextent\b/is) {
		return 1;
	} elsif ($line =~ /\S+\s+inside\s+\S+/is) {
		return 1;
	} elsif ($line =~ /\bor\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\bexpand\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\bexpand\s+text\b/is) {
		return 1;
	} elsif ($line =~ /\brectangle\b/is) {
		return 1;
	} elsif ($line =~ /\benclose\s+rectangle\b/is) {
		return 1;
	} elsif ($line =~ /\btouch\s+(in|inside)\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\b(in|inside)\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\b(out|outside)\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\btouch\s+(in|inside)\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\btouch\s+(out|outside)\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\bnet\s+area\s+ratio\b/is) {
		return 1;
	} elsif ($line =~ /\btouch\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\btouch\b/is) {
		return 1;
	} elsif ($line =~ /\b(coincident|coin)\s+(inside|in)\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\b(coincident|coin)\s+(outside|out)\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\b(coincident|coin)\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\bwith\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\bwith\s+width\b/is) {
		return 1;
	} elsif ($line =~ /\bwith\s+text\b/is) {
		return 1;
	} elsif ($line =~ /\bconvex\s+edge\b/is) {
		return 1;
	} elsif ($line =~ /\bflatten\b/is) {
		return 1;
	} elsif ($line =~ /\bcopy\b/is) {
		return 1;
	} elsif ($line =~ /\bstamp\b/is) {
		return 1;
	} elsif ($line =~ /\bextent\s+cell\b/is) {
		return 1;
	} elsif ($line =~ /\bmerge\b/is) {
		return 1;
	} elsif ($line =~ /\bdonut\b/is) {
		return 1;
	} elsif ($line =~ /\brectangle\s+enclosure\b/is) {
		return 1;
	} elsif ($line =~ /\bshift\s+\S+\b/is) {
		return 1;
	} elsif ($line =~ /\bperimeter\s+\S+\b/is) {
		return 1;
	} elsif ($line =~ /\bwith\s+neighbor\s+\S+\b/is) {
		return 1;
	} elsif ($line =~ /\bdfm\s+space\s+\S+\b/is) {
		return 1;
	} elsif ($line =~ /\bdfm\s+property\s+\S+\b/is) {
		return 1;
	} else {
		return 0;
	}
}

sub addNewGoodCommand {
	my ($goodLine, $currFlag) = @_;
	if (scalar(@lineBuffer) >= 0) {
		my $chkLine;
		my $i = 0; 
		for $chkLine (@lineBuffer) {
			my $chkFlag = $flagBuffer[$i];
			my $comment = $commentBuffer[$i];
			my %ppHash = ();
			&initHashFlag(\%ppHash);

			&loadHashFlag(\%ppHash, $chkFlag);
			my ($transRes, $isStack, $err) = ("", 0, 0);	
			($transRes, $isStack, $err, $chkLine) = &transGenericStatement($chkLine, 0, 0, \%ppHash);	
			if ($err == 0) {
				&printLine($transRes, \%ppHash);
				print $comment;
			} else {
				return;
			}
			++$i;
		}
		@lineBuffer = ($goodLine);
		@flagBuffer = ($currFlag);
		@commentBuffer = ("");
	}
}

#/-------1---------2---------3---------4---------5---------6---------7---------8
sub appendBadCommand {
	my ($badLine, $currFlag) = @_;
	my $num = scalar(@lineBuffer);
	#append to last line
	if ($num > 0) {
		my $lastIndex = $num - 1;
		chomp $lineBuffer[$lastIndex];
		$lineBuffer[$lastIndex] .= " $badLine ";
	} else {
		$lineBuffer[0] = "$badLine";
		@flagBuffer = ($currFlag);
		@commentBuffer = ("");
	}
}

#/-------1---------2---------3---------4---------5---------6---------7---------8
sub appendComment {
	my ($comment) = @_;
	my $lineNum = scalar(@lineBuffer);
	my $i;
	my $commentLineNum = scalar(@commentBuffer);
	for ($i = 0; $i < $lineNum; ++$i) {
		if ($i >= $commentLineNum) {
			$commentBuffer[$i] = "";
		}
	}
	if ($lineNum > 0) {
		my $lastIndex = $lineNum - 1;
		$commentBuffer[$lastIndex] .= $comment;
	} else {
		print $comment;
	}
}
		

#/-------1---------2---------3---------4---------5---------6---------7---------8
sub outAllCommand {
	my $numBuffer = scalar(@lineBuffer);
	if ($numBuffer > 0) {
		my $chkLine;
		my $i = 0; 
		for $chkLine (@lineBuffer) {
			my $chkFlag = $flagBuffer[$i];
			my $comment = $commentBuffer[$i];
			++$i;
			my %ppHash = ();
			&initHashFlag(\%ppHash);
			&loadHashFlag(\%ppHash, $chkFlag);
			my ($transRes, $err) = ("", 0, 0);
			($transRes, $err, $chkLine) = &transGenericStatement($chkLine, 0, 0, \%ppHash);	
			&printLine($transRes, \%ppHash);
			print $comment;
			if ($i == $numBuffer) {
				if ($ppHash{inRule} == 1) {
					$parseFlag{inRule} = 1;
				}
			}
		}
	}
	@lineBuffer = ();
	@flagBuffer = ();
}

sub initHashFlag {
	my ($refFlag) = @_;
	%{$refFlag} = (
			"inRule" => 0,
			"inExp" => 0,
			"firstLineInRule" => 0,
			"incomplete" => 0,
			"inRuleCap" => 0,
			"inTvf" => 0,
			"tmpId" => 0,
	);
};

sub saveHashFlag {
	my ($refFlag) = @_;
	my $flag = 0;
	if (${$refFlag}{inRule} == 1) {
		$flag += 0x80;
	} 
	if (${$refFlag}{inExp} == 1) {
		$flag += 0x40;
	} 
	if (${$refFlag}{firstLineInRule} == 1) {
		$flag += 0x20;
	} 
	if (${$refFlag}{incomplete} == 1) {
		$flag += 0x10;
	} 
	if (${$refFlag}{inRuleCap} == 1) {
		$flag += 0x08;
	} 
	if (${$refFlag}{inTvf} == 1) {
		$flag += 0x04;
	}
	return $flag;
}

sub loadHashFlag {
	my ($refFlag, $flag) = @_;
	if (($flag & 0x80) != 0) {
		${$refFlag}{inRule} = 1;
	}
	if (($flag & 0x40) != 0) {
		${$refFlag}{inExp} = 1;
	}
	if (($flag & 0x20) != 0) {
		${$refFlag}{firstLineInRule} = 1;
	}
	if (($flag & 0x10) != 0) {
		${$refFlag}{incomplete} = 1;
	}
	if (($flag & 0x08) != 0) {
		${$refFlag}{inRuleCap} = 1;
	}
	if (($flag & 0x04) != 0) {
		${$refFlag}{inTvf} = 1;
	}
}
sub formatLine {
	my ($refLine) = @_;
	$$refLine =~ s/ +/ /g;
	$$refLine =~ s/\s+;/;/s;
}

sub printLine {
	my ($pline, $refPrintFlag) = @_;
	&formatLine(\$pline);
	if ($pline =~ /^rule.*\{/) {
		if ($ruleComment =~ /\S/) {
			chomp $pline;
			$pline .= "\t$ruleComment\n";
			$ruleComment = "";
		}
	}
	if ((${$refPrintFlag}{inRule} == 1) and
		(${$refPrintFlag}{inRuleCap} == 0)) {
		print "\t$pline";
	} else {
		print "$pline";
	}
}

sub transRelOp {
	my ($op) = @_;
	my $sumRes = "";
	my $res = "";
	my $haveMatch = 0;
	while ($op =~ /\S/) {
		if ($op =~ s/^<=//) {
			$res = "-le";
			++$haveMatch;
		} elsif ($op =~ s/^<//) {
			$res = "-lt";
			++$haveMatch;
		} elsif ($op =~ s/^>=//) {
			$res = "-ge";
			++$haveMatch;
		} elsif ($op =~ s/^>//) {
			$res = "-gt";
			++$haveMatch;
		} elsif ($op =~ s/^==//) {
			$res = "-eq";
			++$haveMatch;
		} elsif ($op =~ s/^!=//) {
			$res = "-ne";
			++$haveMatch;
		} elsif ($haveMatch > 0 and $op =~ /^[0-9a-zA-Z]/) {
			my ($value,$opRemain) = ($op =~ /^([0-9\.+\-*\/a-z_A-Z]+)(.*)$/);
			$res = "$value";
			$op = $opRemain;
		} elsif ($haveMatch > 0 and $op =~ /^\s*\([^)]+\)\s*/) {
			my ($value, $opRemain) = ($op =~ /^\s*(\([^)]+\))(.*)$/);
			$res = "$value";	
			$op = $opRemain;
		} else {
			$res = "$op";
			$haveMatch = 0;
		}
		if ($haveMatch > 0) {
			$sumRes .= " $res";
		} else {
			last;
		}
	}
	if ($haveMatch > 1) {
		if ($haveMatch == 2) {
			$sumRes =~ s/^\s+//;
			$sumRes =~ s/\s+$//;
			my @wordList = split (/\s+/, $sumRes);
			$wordList[0] =~ s/^-//;
			$wordList[2] =~ s/^-//;
			if ($wordList[0] =~ /^g/) {
				($wordList[2], $wordList[0]) = ($wordList[0], $wordList[2]);	
			}	
			$sumRes = "-".$wordList[0].$wordList[2];
			if ($wordList[1] > $wordList[3]) {
				$sumRes .= " $wordList[3] $wordList[1]";
			} else {
				$sumRes .= " $wordList[1] $wordList[3]";
			}
		} else {
			$sumRes = "!!$sumRes!!";
		}
	} elsif ($haveMatch != 1) {
		$sumRes = $res;
	}
	return ($haveMatch, $sumRes);
}

sub transRptRelOp {
	my ($content, $inLine) = @_;
	my $word;
	my $remain = "";
	($word, $remain) = ($content =~ /^(\S+)(.*)$/is);	
	my ($isOp, $resOp) = &transRelOp($word);
	if ($isOp > 0) {
		return ($resOp, $remain);
	} else {
		print "!! ERROR rel parsing error $word in $inLine\n";
		return ($word, $remain);
	}
}

sub transAbutRelOp {
	my ($op) = @_;
	my $ori = $op;
	my $sumRes = "";
	my $res = "";
	my $haveMatch = 0;
	while ($op =~ /\S/) {
		if ($op =~ s/^<=//) {
			$res = "le";
			++$haveMatch;
		} elsif ($op =~ s/^<//) {
			$res = "lt";
			++$haveMatch;
		} elsif ($op =~ s/^>=//) {
			$res = "ge";
			++$haveMatch;
		} elsif ($op =~ s/^>//) {
			$res = "gt";
			++$haveMatch;
		} elsif ($op =~ s/^==//) {
			$res = "eq";
			++$haveMatch;
		} elsif ($op =~ s/^!=//) {
			$res = "ne";
			++$haveMatch;
		} elsif ($haveMatch > 0 and $op =~ /^[\-.0-9]+/) {
			my ($value,$opRemain) = ($op =~ /^([\-.0-9]+)(.*)$/);
			$res = "$value";
			$op = $opRemain;
		} else {
			$res = "$op";
			$haveMatch = 0;
		}
		if ($haveMatch > 0) {
			$sumRes .= " $res";
		} else {
			last;
		}
	}
	if ($haveMatch > 1) {
		if ($haveMatch == 2) {
			$sumRes =~ s/^\s+//;
			$sumRes =~ s/\s+$//;
			my @wordList = split (/\s+/, $sumRes);
			my $numWord = scalar(@wordList);
			$wordList[0] =~ s/^-//;
			$wordList[2] =~ s/^-//;
			if ($wordList[0] =~ /^g/) {
				($wordList[2], $wordList[0]) = ($wordList[0], $wordList[2]);	
			}	
			$sumRes = $wordList[0].$wordList[2];
			if ($numWord == 4 and $wordList[1] > $wordList[3]) {
				($wordList[3], $wordList[1]) = ($wordList[1], $wordList[3]);	
			}
			$sumRes .= " $wordList[1] ";
			if ($numWord == 4) {	
				$sumRes .= $wordList[3];
			}
		} else {
			$sumRes = "!!$sumRes!!";
		}
	} elsif ($haveMatch != 1) {
		$sumRes = $res;
	}
	return ($haveMatch, $sumRes);
}

sub stackRecover {
	my ($pLine, $pRepCont, $pRepKey) = @_;
	while (scalar(@{$pRepCont}) > 0) {
		if (scalar(@{$pRepKey}) <= 0) {
			last;
		}
		my $content = pop @{$pRepCont};
		my $key	 = pop @{$pRepKey};
		${$pLine} =~ s/$key/($content)/;
	#	${$pLine} = &replaceKeywords(${$pLine});
	}
}

sub extractOutLayer {
	my ($inLine, $pRemain) = @_;
	my $out = "";
	if ($inLine =~ /\S+\s*=\s*.*/s) {
		($out, ${$pRemain}) = ($inLine =~ /(\S+)\s*=\s*(.*)$/s);
	} else {
		${$pRemain} = $inLine;
	}
	return $out;
}

sub transStackStatement {
	my ($line, $fin, $refParseFlag) = @_;
	#print "\n// FROM: $line";
	my @repList = ();
	my @resRepList = ();
	my @resRepKey = ();
	my @repKey = ();
	my $repId = 0;
	my $repT = "__PPEENNTT_";
	my $repTag = $repT.$repId;	
	my $haveStackCmd = 0;
	while ($line =~ /\(/) {
		if ($line =~ /\([^\(\)]*\)/) {
			my ($content) = ($line =~ /\(([^\(\)]*)\)/);
			if (&isCommandInParenthesis($content)) {
				++$haveStackCmd;
				my ($repContent) = ($line =~ s/\(([^\(\)]*)\)/ $repTag /);
				my ($transRes, $err) = &translateRule($content, \%parseFlag);
				&formatLine(\$transRes);
				$transRes =~ s/;\s*$//s;
				push (@repList, $content);
				push (@repKey, $repTag);
				push (@resRepList, $transRes);
				push (@resRepKey, $repTag);
				++$repId;
				$repTag = $repT.$repId;	
				$haveStackCmd = 1;
			} else {
				my ($repContent) = ($line =~ s/\(([^\(\)]*)\)/$repTag/);
				$content = &replaceKeywords($content);
				push (@repList, $content);
				push (@repKey, $repTag);
				push (@resRepList, $content);
				push (@resRepKey, $repTag);
				++$repId;
				$repTag = $repT.$repId;	
			}
		} else {
			if ($fin != 0) {
				$line .= <$fin>;
			} else {
				print "!!CHK_STACK !!INLINE_BEGIN<<$line>>INLINE_END!!\n";
			}
		}
	}
	#print "// >> $line\n";
	#&stackRecover(\$line, \@repList, \@repKey);
	if ($haveStackCmd > 0) {
		my ($transRes, $err) = &translateRule($line, $refParseFlag);
		my $flag = &saveHashFlag($refParseFlag);
		&stackRecover(\$transRes, \@resRepList, \@resRepKey);
		&stackRecover(\$line, \@repList, \@repKey);
		return ($transRes, 	1, $err, $line);
		#&printLine($transRes, \%parseFlag);
	} else {
		my $flag = &saveHashFlag($refParseFlag);
		&stackRecover(\$line, \@repList, \@repKey);
		my ($transRes, $err) = &translateRule($line, $refParseFlag);
		return ($transRes, 0, $err, $line);
	}
}	

sub transSingleStatement {
	my ($line, $refParseFlag) = @_;
	my $flag = &saveHashFlag($refParseFlag);
	my ($transRes, $err) = &translateRule($line, $refParseFlag);
	if ($err != 0) {
		&appendBadCommand($line, $flag);
	} else {
		&addNewGoodCommand($line, $flag);
	}	
}
	
sub transGenericStatement {
	my ($line, $fin, $isExpr, $refParseFlag) = @_;
	my $res = "";
	my $isStack = 0;
	my $err = 0;
	
	my $cline = $line;
	if ($cline =~ /\[[^\[\]]+\]/is) {
		$cline =~ s/\[[^\[\]]+\]//is;
	}
	if ($isExpr == 0 && $cline =~ /\(/) {
		($res, $isStack, $err, $line) = &transStackStatement($line, $fin, $refParseFlag);
		$isStack = 1; 
	} else {
		if ($isExpr == 0 or $cline =~ /^\s*litho\s+file\s+\S+/is )  {
			($res, $err) = &translateRule($line, $refParseFlag);
			&formatLine(\$res);
		} else {
			$err = 1;
			my $cline = $line;
			$cline =~ s/\[.*\]//is;
			if ($cline =~ /[()]/) {
				$isStack = 1;
			} else {
				$isStack = 0;
			}
			$res = $line;	
		}
	}
	return ($res, $isStack, $err, $line);
}
	
sub getCompleteValue {
	my ($refIn) = @_;
	my $in = $$refIn;
	my $value = "";
	my $word = "";
	my $needValue = 1;
	my $space = "";
	my $good = 1;
	while ($in =~ /\S+/) {
		($space, $in) = ($in =~ /^(\s*)(.*)/is);
		if ($needValue == 1) {
			if ($in =~ /^[a-z_A-Z\$][a-zA-Z_0-9]*/is) {
				($word, $in) = ($in =~ /^([a-z_A-Z\$][a-zA-Z_0-9]*)(.*)/is);
				$value .= ($space.$word);
			} elsif ($in =~ /^[\-0-9.][0-9.]*/is) {
				($word, $in) = ($in =~ /^([\-0-9.][0-9.]*)(.*)/is);
				$value .= ($space.$word);
			} elsif ($in =~ /^\([^\)]+\)/) {
				($word, $in) = ($in =~ /^(\([^\)]+\))(.*)/is);
				$value .= ($space.$word);
			} else {
				print "!!CHK_VALUE when get value from $$refIn\n";
			}
			$needValue = 0;	
		} elsif ($needValue == 0) {
			if ($in =~ /^[+\-*\/]/) {
				($word, $in) = ($in =~ /^([+\-*\/])(.*)/is);
				$value .= ($space.$word);
				$needValue = 1;	
			} else {
				$in = $space.$in;
				last;
			}
		}
	}
	$$refIn = $in;
	return ($value, $good); 
}	

sub parseCmd {
	my ($inLine, $key) = @_;
	my ($preKey, $remain) = ($inLine =~ /^\s*(.*)\b$key\b(.*)$/is);
	my $outLay = "";
	my $aftE = "";
	if ($preKey =~ /=/) {
		$outLay = &extractOutLayer($preKey, \$aftE);
	} else {
		$aftE = $preKey;
	}
	if ($aftE =~ /\S+/) {
		$remain = "$aftE $remain";
	}
	return ($outLay, $remain);
}
	
sub parseCmdWithLayer {
	my ($inLine, $key) = @_;
	my ($preKey, $layer, $remain) = ("", "", "");
	if ($key =~ /\(.*\)/is) {
		my $matchKey = "";
		($preKey, $matchKey, $layer, $remain) = ($inLine =~ /^\s*(.*?)\b$key\s+(\S+)(.*)$/is);
	} elsif ($inLine =~ /\s*\b$key\s*$/is) {
		($preKey, $remain) = ($inLine =~ /^\s*(.*?)\b$key\s*(.*)$/is);
	} else {
		($preKey, $layer, $remain) = ($inLine =~ /^\s*(.*?)\b$key\s+(\S+)(.*)$/is);
	}
	my $outLay = "";
	my $aftE = "";
	if ($preKey =~ /=/) {
		$outLay = &extractOutLayer($preKey, \$aftE);
	} else {
		$aftE = $preKey;
	}
	my $layerFront = 0;
	if ($aftE =~ /\S+\s*$/) {
		my $realLayer = "";
		($aftE, $realLayer) = ($aftE =~ /^(.*?)(\S+)\s*$/);
		$remain = "$layer $remain";
		$layer = $realLayer;
		$layerFront = 1;
	}
	if ($aftE =~ /\S+/) {
		print "!!CHK_ERR_AFT_CMD_WITHLAYER<<$aftE>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
	}
	if ($layerFront == 0 and $layer =~ /^\S+[<>=!]/) {
		my ($word, $expr) = ($layer =~ /^([^<>=!]+)(.*)$/is);
		$layer = $word;
		$remain = " $expr $remain";
	}

	return ($outLay, $layer, $remain);
}

sub parseCmdWithNotLayer {
	my ($inLine, $key) = @_;
	my ($preKey, $layer, $remain) = ("", "", "");
	if ($key =~ /\(.*\)/is) {
		my $matchKey = "";
		($preKey, $matchKey, $layer, $remain) = ($inLine =~ /^\s*(.*?)\b$key\s+(\S+)(.*)$/is);
	} else {
		($preKey, $layer, $remain) = ($inLine =~ /^\s*(.*?)\b$key\s+(\S+)(.*)$/is);
	}
	if (not defined($preKey)) {
		return ("", "", "", "");
	}
	my $outLay = "";
	my $aftE = "";
	my $notFlag = "";
	if ($preKey =~ /not\s*$/is) {
		$preKey =~ s/not\s*$//is;
		$notFlag = " -not";
	}
	if ($preKey =~ /=/) {
		$outLay = &extractOutLayer($preKey, \$aftE);
	} else {
		$aftE = $preKey;
	}
	my $layerFront = 0;
	if ($aftE =~ /\S+\s*$/) {
		my $realLayer = "";
		($aftE, $realLayer) = ($aftE =~ /^(.*?)(\S+)\s*$/);
		$remain = "$layer $remain";
		$layer = $realLayer;
		$layerFront = 1;
	}
	if ($aftE =~ /\S+/) {
		print "!!CHK_ERR_AFT_WITH_NOT_LAYER<<$aftE>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
	}
	if ($layerFront == 0 and $layer =~ /^\S+[<>=!]/) {
		my ($word, $expr) = ($layer =~ /^([^<>=!]+)(.*)$/is);
		$layer = $word;
		$remain = " $expr $remain";
	}
	return ($outLay, $notFlag, $layer, $remain);
}
	
sub checkEmpty {
	my ($word, $inLine) = @_;
	if ($word =~ /\S+/) {
		print "!!CHK_ERR<<$word>> in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
	}
}

sub getLayer {
	my ($in) = @_;
	my $left = "";
	my $word = "";
	if (&isGoodLayerName($in)) {
		($word, $left) = ($in =~ /^([0-9a-zA-Z_\$\.:]+)(.*)$/is);
		return ($word, $left, 1);
	} elsif ($in =~ /^\(\s*[^\)]+\s*\)/) {
		($word, $left) = ($in =~ /^\(\s*([^\)]+)\s*\)(.*)$/is);
		if (&isGoodLayerName($word)) {
			return ("($word)", $left, 1);
		} else {
			return ("", $in, 0);
		}
	} elsif ($in =~ /^\[\s*[^\]]+\s*\]/) {
		($word, $left) = ($in =~ /^\[\s*([^\]]+)\s*\](.*)$/is);
		if (&isGoodLayerName($word)) {
			return ("[$word]", $left, 1);
		} else {
			return ("", $in, 0);
		}
	} else { 
		return ("", $in, 0);
	}		
}

sub isGoodLayerName {
	my ($word) = @_;
	if ($word =~ /^[a-zA-Z_\$0-9\.:]+/) {
		return 1;
	} else {
		return 0;
	}
}
sub checkIsExpress {
	my ($inLine) = @_;
	if ($inLine !~ /[\[\]]/is) {
		return 0;
	}
	if ($inLine =~ /^[^\[\]]*\[/is) {
		my ($preB, $remain) = ($inLine =~ /^([^\[\]]*)(\[.*)$/is);
		if ($preB =~ /\w\w/is) {
			return 0;
		} else {
			return 1;
		}	
	}
	return 0;
}

sub replaceKeywords {
	my ($in) = @_;
	my $out = $in;
	if ($out =~ /\bgrid\b/is and $out !~ /grid:.*\{/is) {
		$out =~ s/\bgrid\b/replace_grid/gis;
	} elsif ($out =~ /\bantenna\b/is and $out !~ /\.antenna/is and $out !~ /antenna\./is ) {
		$out =~ s/\bantenna\b/replace_antenna/gis;
	}
	return $out;
}
	
sub takeLastWord {
	my $lastIndex = scalar(@lineBuffer);
	my $res = "";
	if ($lastIndex == 0) {
		return $res;
	} else {
		--$lastIndex;
	}
	my $lastLine = $lineBuffer[$lastIndex];
	($lastLine, $res) = ($lastLine =~ /^(.*?)(\S+)\s*$/is);
	$lineBuffer[$lastIndex] = $lastLine;
	return $res;		
}

sub isAssignment {
	my ($inLine) = @_;
	if ($inLine =~ /\[.*\]/) {
		$inLine =~ s/\[.*\]//is;
	}
	if ($inLine !~ /=/) {
		return 0;
	} elsif ($inLine =~ /[^<>!=]=[^=]/) {
		if ($inLine =~ /^[^\[\]]*=/) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
} 

sub isSquareBracket {
	my ($inLine, $openBracket) = @_;
	my $openLeft = $openBracket;
	while ($inLine =~ /\/\/.*$/) {
		$inLine =~ s/\/\/.*$//;
	}
	my $haveBracket = 0;
	while ($inLine =~ /[\[\]]/) {
		$haveBracket = 1;
		if ($inLine =~ /^[^\[\]]*\[/) {
			++$openLeft;
			$inLine =~ s/^[^\[\]]*\[//;
		} elsif ($inLine =~ /^[^\[\]]*\]/) {
			--$openLeft;
			$inLine =~ s/^[^\[\]]*\]//;
		} else {
			last;
		}
	}
	return ($haveBracket, $openLeft);
}

sub isFuncDefBegin {
	my ($inLine) = @_;
	if ($inLine =~ /^\s*tvf\s+function\s+\S+\s*\[\s*\/\*\s*/is) {
		return 1;
	} elsif ($inLine =~ /^\s*dfm\s*function\s+\[\s*/is) {
		return 1;
	} else {
		return 0;
	}
}

sub loadFullFunction {
	my ($inLine, $fin) = @_;
	my $openBkt = &getOpenBkt($inLine, 0);
	while ($openBkt != 0) {
		my $newLine = "";
		if ($fin != 0) {
			$newLine = <$fin>;
		} else {
			print "!!CHK_MISSING_CLOSE_BASKET in !!INLINE_BEGIN<<$inLine>>INLINE_END!!\n";
		}
		$openBkt = &getOpenBkt($newLine, $openBkt);
		$inLine .= $newLine;
	}
	return $inLine;
}

sub getOpenBkt {
	my ($inLine, $openBkt) = @_;
	my $chkLine = $inLine;
	while ($inLine =~ /[\[\]]/) {
		my ($before, $bkt, $after) = ($inLine =~ /^([^\[\]]*)([\[\]])(.*)$/); 
		if ($bkt eq "[")  {
			++$openBkt;
		} elsif ($bkt eq "]") {
			--$openBkt;
		} else {
			print "error $bkt in $chkLine\n";
		}
		$inLine = $after;
	}
	return $openBkt;
}

sub genOutLine {
	my ($key, $content, $outLay, $flag) = @_;
	my $res = "";
	my $body = "$key $content";
	$lastCmd{key} = "$key";
	$lastCmd{content} = $content;
	$lastCmd{outLay} = $outLay;
	if ($flag <= 0) {
		$flag = 0;
	}
	if ($outLay !~ /\S+/) {
		$res = "$body";
	} else {
		if ($flag > 0) {
			$res = "$body -o $outLay";
		} else {
			$res = "$body $outLay";
		}
	}
	$res .= ";\n";
	return $res;
}
	
sub isComplexCmd {
	my ($res) = @_;
	if ($res =~ /^antenna\s+/) {
		return 1;
	} elsif ($res =~ /^dfm_property\s+/) {
		return 1;
	} elsif ($res =~ /^dfm_copy\s+/) {
		return 1;
	} elsif ($res =~ /^dfm_rdb\s+/) {
		return 1;
	} elsif ($res =~ /^rect_enc\s+/) {
		return 1;
	} else {
		return 0;
	}
}

sub isKeywords {
	my ($content) = @_;
	if ($content =~ /^not\b/is) {
		return 1;
	} elsif ($content =~ /^and\b/is) {
		return 1;
	} elsif ($content =~ /^or\b/is) {
		return 1;
	} else {
		return 0;
	}
}
	

sub isCmdNeedSquareExp {
	my ($refCmd) = @_;
	if (not exists $$refCmd{key}) {
		return 0;
	} 
	if ($$refCmd{key} eq "dfm_property_net" or
		$$refCmd{key} eq "dfm_property" or
		$$refCmd{key} eq "dfm_property_merge") {
		return 1;
	} else {
		return 0;
	}
}

sub finishSquareExp {
	my ($refCmd, $rf, $rline, $isExpr, $refParseFlag) = @_;
	if ($bpLevel > 0) {
		if ($rline =~ /\]/) {
			--$bpLevel;
			$$refCmd{expr} .= "\n";
		}
		$$refCmd{expr} .= $rline;
		return;	
	}
	my ($preBp, $bpRemain) = ($rline =~ /^([^\[]*)(\[.*$)/);
	if (not defined $bpRemain) {
		print "!!CHK_MISSING_SQUARE_BRACKET<<$rline>>\n";
		return;
	}
	my $res = $preBp;
	if ($preBp =~ /\S+/) {
		$res .= "\n";
	} elsif ($bpLevel == 0) {
		$res .= "\n";
	}
	my $fullBpRange = "";
	while ($bpRemain =~ /^\[[^\[]*\]/) {
		($fullBpRange, $bpRemain) = ($bpRemain =~ /^(\[[^\[]*\])(.*)$/);
		
		$bpRemain =~ s/^\s*//;
		$res .= "$fullBpRange\n";
	}
	$res .= $bpRemain;
	$$refCmd{expr} .= "$res\n";
	if ($bpRemain =~ /\[[^\]]*$/) {
		++$bpLevel;	
	} elsif ($bpRemain =~ /[^\]]*\]/) {
		--$bpLevel;
	}
}

sub appendExpr {
	my ($expr) = @_;
	my $numBuf = scalar(@lineBuffer);
	if ($numBuf == 0) {
		return;
	}
	my $lastOne = pop @lineBuffer;
	$lastOne .= "\n$expr";
	push @lineBuffer, $lastOne;
}
