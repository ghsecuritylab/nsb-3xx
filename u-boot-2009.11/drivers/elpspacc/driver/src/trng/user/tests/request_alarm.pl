#!/usr/bin/env perl

=head1 SYNOPSIS

generation_increment.pl [-d device]

Tests that the request alarm is raised after the configured number of
requests is reached.

=cut

use strict;

use TRNG3::Testlib;

TRNG3::Testlib::init();
TRNG3::Testlib::sysfs_open();

my $seed = TRNG3::Testlib::open_attribute("seed", ">");
my $reseed_action = TRNG3::Testlib::open_attribute("reseed_action", ">");
my $wd_requests = TRNG3::Testlib::open_attribute("wd_requests", "+>");
my $wd_alarm = TRNG3::Testlib::open_attribute("wd_alarm", "<");
my $random = TRNG3::Testlib::open_attribute("random", "<");

my $request_count = 42;
TRNG3::Testlib::write_attribute($wd_requests, $request_count);
$request_count = TRNG3::Testlib::read_attribute($wd_requests);

TRNG3::Testlib::write_attribute($reseed_action, "random");
for (my $i = 0; $i < $request_count - 1; $i++) {
   TRNG3::Testlib::read_attribute($random);
}

my ($first, $second);
$first = TRNG3::Testlib::read_attribute($wd_alarm);
TRNG3::Testlib::read_attribute($random);
$second = TRNG3::Testlib::read_attribute($wd_alarm);

if ($second ne "request") {
   TRNG3::Testlib::fail("Request alarm not detected after random reseed\n"
                       ."expected: request\n"
                       ."actual: $second\n");
}

if ($first eq $second) {
   TRNG3::Testlib::fail("Alarm was raised prematurely\n");
}

TRNG3::Testlib::write_attribute($seed, "5a" x 32);
TRNG3::Testlib::write_attribute($reseed_action, "nonce");
for (my $i = 0; $i < $request_count - 1; $i++) {
   TRNG3::Testlib::read_attribute($random);
}

$first = TRNG3::Testlib::read_attribute($wd_alarm);
TRNG3::Testlib::read_attribute($random);
$second = TRNG3::Testlib::read_attribute($wd_alarm);

if ($second ne "request") {
   TRNG3::Testlib::fail("Request alarm not detected after nonce reseed\n"
                       ."expected: request\n"
                       ."actual: $second\n");
}

if ($first eq $second) {
   TRNG3::Testlib::fail("Alarm was raised prematurely\n");
}

TRNG3::Testlib::pass();
