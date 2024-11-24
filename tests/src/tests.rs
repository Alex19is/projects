use assertables::assume;
use rand::prelude::*;
use rand_pcg::Pcg64;

use anysystem::test::TestResult;

use crate::common::{build_system, check_guarantees, check_overhead, send_messages, TestConfig};

pub fn test_normal(config: &TestConfig) -> TestResult {
    let mut sys = build_system(config, false);
    let messages = send_messages(&mut sys, 5);
    sys.step_until_no_events();
    check_guarantees(&mut sys, &messages, config)?;
    // we expect no more than 5 messages from sender in ideal network conditions
    let sent_count = sys.sent_message_count("sender");
    assume!(
        sent_count <= 5,
        format!("Sender sent {} messages, expected at most 5", sent_count)
    )
}

pub fn test_normal_non_unique(config: &TestConfig) -> TestResult {
    let mut sys = build_system(config, false);
    let messages = send_messages(&mut sys, 10);
    sys.step_until_no_events();
    check_guarantees(&mut sys, &messages, config)?;
    // we expect no more than 10 messages from sender in ideal network conditions
    let sent_count = sys.sent_message_count("sender");
    assume!(
        sent_count <= 10,
        format!("Sender sent {} messages, expected at most 10", sent_count)
    )
}

pub fn test_delayed(config: &TestConfig) -> TestResult {
    let mut sys = build_system(config, false);
    sys.network().set_delays(1., 3.);
    let messages = send_messages(&mut sys, 5);
    sys.step_until_no_events();
    check_guarantees(&mut sys, &messages, config)
}

pub fn test_duplicated(config: &TestConfig) -> TestResult {
    let mut sys = build_system(config, false);
    sys.network().set_dupl_rate(0.3);
    let messages = send_messages(&mut sys, 5);
    sys.step_until_no_events();
    check_guarantees(&mut sys, &messages, config)
}

pub fn test_delayed_duplicated(config: &TestConfig) -> TestResult {
    let mut sys = build_system(config, false);
    sys.network().set_delays(1., 3.);
    sys.network().set_dupl_rate(0.3);
    let messages = send_messages(&mut sys, 5);
    sys.step_until_no_events();
    check_guarantees(&mut sys, &messages, config)
}

pub fn test_dropped(config: &TestConfig) -> TestResult {
    let mut sys = build_system(config, false);
    sys.network().set_drop_rate(0.3);
    let messages = send_messages(&mut sys, 5);
    sys.step_until_no_events();
    check_guarantees(&mut sys, &messages, config)
}

pub fn test_chaos_monkey(config: &TestConfig) -> TestResult {
    let mut rand = Pcg64::seed_from_u64(config.seed);
    for i in 1..=config.monkeys {
        let mut run_config = *config;
        run_config.seed = rand.next_u64();
        println!("Run {} (seed: {})", i, run_config.seed);
        let mut sys = build_system(&run_config, false);
        sys.network().set_delays(1., 3.);
        sys.network().set_dupl_rate(0.3);
        sys.network().set_drop_rate(0.3);
        let messages = send_messages(&mut sys, 10);
        sys.step_until_no_events();
        let res = check_guarantees(&mut sys, &messages, &run_config);
        res.as_ref()?;
    }
    Ok(true)
}

pub fn test_overhead(config: &TestConfig, guarantee: &str, faulty: bool) -> TestResult {
    for message_count in [100, 500, 1000] {
        let mut sys = build_system(config, true);
        if faulty {
            sys.network().set_delays(1., 3.);
            sys.network().set_dupl_rate(0.3);
            sys.network().set_drop_rate(0.3);
        }
        let messages = send_messages(&mut sys, message_count);
        sys.step_until_no_events();
        let res = check_guarantees(&mut sys, &messages, config);
        res.as_ref()?;
        let sender_mem = sys.max_size("sender");
        let receiver_mem = sys.max_size("receiver");
        let net_message_count = sys.network().network_message_count();
        let net_traffic = sys.network().traffic();
        println!(
            "{:<6} Send Mem: {:<8} Recv Mem: {:<8} Messages: {:<8} Traffic: {}",
            message_count, sender_mem, receiver_mem, net_message_count, net_traffic
        );
        check_overhead(
            guarantee,
            faulty,
            message_count,
            sender_mem,
            receiver_mem,
            net_message_count,
            net_traffic,
        )?;
    }
    Ok(true)
}
