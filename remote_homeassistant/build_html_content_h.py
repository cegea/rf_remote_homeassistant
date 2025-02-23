header_content : str = ""

with open("include/html_content_template.h", "r") as header_file:
            header_content = header_file.read()

with open("include/html_content.h", "w") as header_file:
    with open("include/provisioning-land-page.html", "r") as provisioning_land_page:
        with open("include/success-page.html", "r") as success_page_html:
            header_content = header_content.replace("@HTML_PAGE", provisioning_land_page.read())
            header_content = header_content.replace("@HTML_SUCCESS_PAGE", success_page_html.read())

    # Write content to header file
    header_file.write(header_content)

