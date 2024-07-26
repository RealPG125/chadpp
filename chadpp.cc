#include <fmt/printf.h>
#include <dpp/dpp.h>
#include <expected>
#include <chrono>
#include <cstdlib>

constexpr uint64_t default_guild_id = 1116375728343228438;

auto get_env(std::string_view env) -> std::expected<std::string, std::runtime_error>
{
	char const* res = std::getenv(env.data());
	if (!res)
		return std::unexpected(std::runtime_error(fmt::format("Expected {}.", env)));
	return std::string(res);
}

void terminate(std::string_view message = "You Fucked Up.")
{
	fmt::fprintf(stderr, fmt::format("Err: {}\n", message));
	std::exit(1);
}

auto main(int argc, char const** argv) -> int
{
	std::string token = get_env("CHADPP_TOKEN").transform_error(
		[](std::runtime_error const& err) { 
			terminate(err.what());
			return "";
		}
	).value();

	fmt::print("Starting Bot,\n\ttoken: {}\n", token);

	dpp::cluster bot(token, dpp::i_all_intents);

    auto start = std::chrono::system_clock::now();

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot](dpp::ready_t const& event) {
        dpp::slashcommand newcommand("embed", "Create an embed", bot.me.id);
        bot.guild_command_create(newcommand, default_guild_id, [] (auto const& ev) { fmt::print("Registered Commands"); });
        fmt::print("Bot is now online!\n");
    });

	bot.on_slashcommand([&bot](dpp::slashcommand_t const& event) {
        /* Check which command they ran */
        if (event.command.get_command_name() == "embed") {
            /* Create an embed */
            dpp::embed embed = dpp::embed()
                .set_color(dpp::colors::sti_blue)
                .set_title("Some name")
                .set_url("https://dpp.dev/")
                .set_author("Some name", "https://dpp.dev/", "https://dpp.dev/DPP-Logo.png")
                .set_description("Some description here")
                .set_thumbnail("https://dpp.dev/DPP-Logo.png")
                .add_field(
                    "Regular field title",
                    "Some value here"
                )
                .add_field(
                    "Inline field title",
                    "Some value here",
                    true
                )
                .add_field(
                    "Inline field title",
                    "Some value here",
                    true
                )
                .set_image("https://dpp.dev/DPP-Logo.png")
                .set_footer(
                    dpp::embed_footer()
                    .set_text("Some footer text here")
                    .set_icon("https://dpp.dev/DPP-Logo.png")
                )
                .set_timestamp(time(0));
 
            /* Create a message with the content as our new embed. */
            dpp::message msg(event.command.channel_id, embed);
 
            /* Reply to the user with the message, containing our embed. */
            event.reply(msg);
        }
    });

    bot.start(dpp::st_return);
    bool running = true;
	while (running) {
		char ch = std::getchar();
		switch (ch) {
		case 'q':
			running = false;
			break;
		default:
			break;
		}
	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> timelapse = end - start;
	fmt::print("Exiting Bot,\n\ttimelapse: {}s\n", timelapse.count());

	return 0;
}
